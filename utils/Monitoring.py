"""
Live telemetry from C++ IEDs over ZMQ inproc (PULL here, PUSH in C++).

Каждое сообщение — один временной шаг: скалярные графики и логические сигналы в graphs;
плоскость R–X — только точки pr/px по фазам на шаг (история траекторий накапливается здесь).
Полигоны зон защиты приходят один раз в первом сообщении сессии.

Хвост рядов времени ограничен TELEMETRY_TAIL_MAX_POINTS (обрезка на стороне Python).
"""
import json
import math
import os
import sys
import time


def _configure_qt_platform_before_matplotlib():
    """
    На Linux Qt 6 по умолчанию может выбрать плагин «wayland»; в WSL / без сокета Wayland
    это даёт «Failed to create wl_display» и падение загрузки qtagg. Тогда задаём X11 (xcb).
    """
    if not sys.platform.startswith("linux") or "QT_QPA_PLATFORM" in os.environ:
        return
    wd = os.environ.get("WAYLAND_DISPLAY")
    if not wd:
        os.environ["QT_QPA_PLATFORM"] = "xcb"
        return
    rt = os.environ.get("XDG_RUNTIME_DIR")
    if not rt:
        os.environ["QT_QPA_PLATFORM"] = "xcb"
        return
    sock = os.path.join(rt, wd)
    if not os.path.exists(sock):
        os.environ["QT_QPA_PLATFORM"] = "xcb"


_configure_qt_platform_before_matplotlib()

import zmq

try:
    import matplotlib

    if sys.platform.startswith("linux"):
        try:
            matplotlib.use("qtagg")
        except Exception:
            matplotlib.use("TkAgg")
    else:
        matplotlib.use("TkAgg")
    import matplotlib.pyplot as plt
    from matplotlib.widgets import Button, RangeSlider
except Exception as exc:  # pragma: no cover
    print("matplotlib:", exc, file=sys.stderr)
    import matplotlib as _mpl

    _mpl.use("TkAgg")
    import matplotlib.pyplot as plt
    from matplotlib.widgets import Button, RangeSlider

try:
    import numpy as np
except ImportError as _e:  # pragma: no cover
    raise SystemExit(
        "Monitoring.py требует пакет numpy (прореживание и хранение рядов). Установите: pip install numpy"
    ) from _e

ctx_addr = zmq_context_address
ctx = zmq.Context.shadow(ctx_addr)

ENDPOINT = "inproc://ied_telemetry"

# --- Временные графики (TelemetryPlotter) ---
# MIN_DRAW_INTERVAL — зарезервировано; перерисовка данных не душится (иначе ось времени «ползёт» медленнее симуляции).
MIN_DRAW_INTERVAL = 0.0
# RELIM_INTERVAL_S — как часто пересчитывать масштаб по Y (relim/autoscale) при движении по времени.
RELIM_INTERVAL_S = 0.7
# MAX_DISPLAY_POINTS_PER_LINE — максимум точек на одну линию в режиме обзора (видно ~весь интервал по времени).
#   Полные данные остаются в памяти; на экран уходит не больше этого числа (главный «рычаг» качества/скорости).
MAX_DISPLAY_POINTS_PER_LINE = 600
# DISPLAY_POINTS_HARD_CAP — верхний предел точек на линию при сильном увеличении (узкое окно по времени).
DISPLAY_POINTS_HARD_CAP = 1200
# USE_ENVELOPE_DOWNSAMPLE — для стиля «line» при прореживании использовать min/max по корзинам (лучше виден синус на обзоре).
USE_ENVELOPE_DOWNSAMPLE = True

# TELEMETRY_RECV_DRAIN_CAP — верхний предел JSON за один «захват» очереди (первый recv блокирующий, дальше NOBLOCK до пустоты).
TELEMETRY_RECV_DRAIN_CAP = 5_000_000
# TELEMETRY_ANIM_UI_EVERY_N — после каждых N принятых сообщений обновить графики (1 = «анимация» по шагам; больше — быстрее догон без отрисовки каждого кадра).
TELEMETRY_ANIM_UI_EVERY_N = max(1, int(os.environ.get("MONITORING_ANIM_UI_EVERY_N", "150")))

# --- Окно R–X ---
# RX_UPDATE_MIN_INTERVAL — после батча: не чаще (с) лишняя перерисовка R–X (во время анимации redraw вызывается каждый шаг).
RX_UPDATE_MIN_INTERVAL = 0.02
# RX_TRAJ_MAX_STORE_POINTS — максимум точек на траекторию (память и линия на графике): при превышении отбрасывается хвост слева; прореживание не применяется.
RX_TRAJ_MAX_STORE_POINTS = 6000
# RX_TRAJ_LEAD_SUPPRESS_OHM — если > 0, не рисовать ведущие точки у (0,0), пока |Z| < порога (0 = не отбрасывать, полная линия).
RX_TRAJ_LEAD_SUPPRESS_OHM = 1e-3
# RX_TRAJ_SEGMENT_BREAK_OHM — если шаг между соседними точками по времени больше (Ом), вставляется разрыв линии (NaN), чтобы не тянуть хорду через КЗ/скачок.
RX_TRAJ_SEGMENT_BREAK_OHM = 1000.0

# PLAYBACK_DELAY_SEC — пауза (с) после каждого сообщения телеметрии (0 = без искусственной задержки).
PLAYBACK_DELAY_SEC = 0.0

# TELEMETRY_TAIL_MAX_POINTS — хвост по времени на граф (общий t); обрезка только если длина > TELEMETRY_TAIL_MAX_POINTS * TELEMETRY_TRIM_TRIGGER_MULT.
TELEMETRY_TAIL_MAX_POINTS = 350000
TELEMETRY_TRIM_TRIGGER_MULT = 2

# IDLE_CYCLES_FOR_DONE — подряд пустых recv до признания симуляции завершённой.
IDLE_CYCLES_FOR_DONE = 80
# FIG_WINDOW_MISSING_DEBOUNCE — сколько циклов подряд «окно не найдено» до выхода (защита от ложных срабатываний).
FIG_WINDOW_MISSING_DEBOUNCE = 16

TIME_PAN_FRACTION = 0.1
TIME_ZOOM_FACTOR = 1.25

sock = ctx.socket(zmq.PULL)
sock.bind(ENDPOINT)
sock.setsockopt(zmq.RCVTIMEO, 50)
sock.setsockopt(zmq.RCVHWM, 0)

if "notify_telemetry_pull_bound" in globals():
    notify_telemetry_pull_bound()


# MONITORING_UI — «pyqtgraph» (PyQtGraph + Qt, быстрее для realtime), «matplotlib» по умолчанию, «auto» — pyqtgraph если пакет есть. Пример: export MONITORING_UI=pyqtgraph
MONITORING_UI_DEFAULT = "matplotlib"


def _terminate_process():
    try:
        from pyqtgraph.Qt import QtWidgets

        app = QtWidgets.QApplication.instance()
        if app is not None:
            app.quit()
    except Exception:
        pass
    try:
        import matplotlib.pyplot as _plt

        _plt.close("all")
    except Exception:
        pass
    os._exit(0)


def _parse_float(s):
    return float(s.strip().replace(",", "."))


def _playback_delay_sec():
    return max(0.0, float(PLAYBACK_DELAY_SEC))


def _trim_graphs_tail(graphs, lim, trigger_mult, plotter=None):
    """Один раз обрезать начало общего t графа и всех y; только при len > lim * trigger_mult (амортизация)."""
    if lim <= 0:
        return
    threshold = lim * max(2, int(trigger_mult))
    trimmed = False
    for g in graphs.values():
        t = g["t"]
        if len(t) <= threshold:
            continue
        cut = len(t) - lim
        if cut <= 0:
            continue
        t[:] = t[cut:]
        for ld in g["lines"].values():
            y = ld["y"]
            if len(y) > cut:
                y[:] = y[cut:]
            else:
                ld["y"] = []
        g["_np_ver"] = int(g.get("_np_ver", 0)) + 1
        trimmed = True
    if trimmed and plotter is not None:
        plotter._recompute_seen_t_bounds()


def _effective_display_max_points(t_list, win):
    """Лимит точек на линию для отрисовки (упрощённая логика)."""
    base = MAX_DISPLAY_POINTS_PER_LINE
    cap = DISPLAY_POINTS_HARD_CAP
    if not t_list:
        return max(2, min(base, cap))
    n_all = len(t_list)
    if n_all < 2:
        return min(cap, max(2, n_all))
    if win is None:
        return min(cap, base)
    tlo, thi = float(win[0]), float(win[1])
    d0, d1 = float(t_list[0]), float(t_list[-1])
    span = max(d1 - d0, 1e-18)
    win_w = max(thi - tlo, 1e-18)
    frac = min(1.0, win_w / span)
    n_in_window = min(n_all, int(n_all * frac) + 8)
    if frac >= 0.95:
        return min(cap, base)
    return min(cap, max(base, n_in_window))


def _rx_strip_leading_near_origin(tr, tx, min_mag):
    """Убрать ведущие точки у (0,0), пока |Z| < min_mag (остаётся хотя бы одна точка при непустом ряде)."""
    if min_mag <= 0.0 or not tr or len(tr) != len(tx):
        return list(tr), list(tx)
    tr, tx = list(tr), list(tx)
    min_sq = min_mag * min_mag
    while len(tr) >= 2:
        r0, x0 = float(tr[0]), float(tx[0])
        if r0 * r0 + x0 * x0 >= min_sq:
            break
        del tr[0], tx[0]
    return tr, tx


def _rx_break_xy_jumps(tr, tx, max_step_ohm):
    """Вставить (nan,nan) между соседними точками с большим шагом в плоскости R–X (разрыв линии в matplotlib)."""
    if max_step_ohm <= 0.0 or len(tr) != len(tx) or len(tr) < 2:
        return list(tr), list(tx)
    msq = max_step_ohm * max_step_ohm
    out_r = [tr[0]]
    out_x = [tx[0]]
    for i in range(1, len(tr)):
        dr = float(tr[i]) - float(tr[i - 1])
        dx = float(tx[i]) - float(tx[i - 1])
        if dr * dr + dx * dx > msq:
            out_r.append(float("nan"))
            out_x.append(float("nan"))
        out_r.append(tr[i])
        out_x.append(tx[i])
    return out_r, out_x


def _compress_step_plateau(t_list, y_list):
    """
    Сжатие ряда для drawstyle steps-post (numpy): плато одинаковых y → одна горизонталь до смены уровня.
    """
    n = len(t_list)
    if n == 0 or len(y_list) != n:
        return list(t_list), list(y_list)
    ta = np.asarray(t_list, dtype=np.float64)
    ya = np.asarray(y_list, dtype=np.float64)
    if n == 1:
        return [float(ta[0])], [float(ya[0])]
    ch = np.flatnonzero(ya[1:] != ya[:-1])
    starts = np.concatenate(([0], ch + 1))
    ends = np.concatenate((ch, [n - 1]))
    ot, oy = [], []
    for k in range(len(starts)):
        i = int(starts[k])
        j = int(ends[k])
        ts = float(ta[i])
        te = float(ta[j + 1]) if j + 1 < n else float(ta[j])
        y = float(ya[i])
        if not ot:
            ot.append(ts)
            oy.append(y)
        ot.append(te)
        oy.append(y)
        if k + 1 < len(starts):
            yn = float(ya[int(starts[k + 1])])
            ot.append(te)
            oy.append(yn)
    return ot, oy


def _envelope_downsample_tt_yy(tt, yy, max_pts):
    """Векторизованное min/max по корзинам; tt, yy — numpy 1d."""
    n = int(len(tt))
    if n <= max_pts:
        return tt.tolist(), yy.tolist()
    n_seg = max(1, max_pts // 2)
    bsz = int(np.ceil(n / float(n_seg)))
    pad = (bsz - (n % bsz)) % bsz
    if pad:
        tt = np.append(tt, np.full(pad, tt[-1], dtype=np.float64))
        yy = np.append(yy, np.full(pad, yy[-1], dtype=np.float64))
    n2 = len(tt)
    n_rows = n2 // bsz
    seg_t = tt.reshape(n_rows, bsz)
    seg_y = yy.reshape(n_rows, bsz)
    idx_min = np.argmin(seg_y, axis=1)
    idx_max = np.argmax(seg_y, axis=1)
    row = np.arange(n_rows, dtype=np.intp)
    t_min = seg_t[row, idx_min]
    t_max = seg_t[row, idx_max]
    y_min = seg_y[row, idx_min]
    y_max = seg_y[row, idx_max]
    mask = idx_min != idx_max
    t_acc = []
    y_acc = []
    for r in range(n_rows):
        if not mask[r]:
            ti, yi = float(t_min[r]), float(y_min[r])
            if not t_acc or ti != t_acc[-1] or yi != y_acc[-1]:
                t_acc.append(ti)
                y_acc.append(yi)
        else:
            p0 = (float(t_min[r]), float(y_min[r]))
            p1 = (float(t_max[r]), float(y_max[r]))
            pairs = (p0, p1) if p0[0] <= p1[0] else (p1, p0)
            for ti, yi in pairs:
                if not t_acc or ti != t_acc[-1] or yi != y_acc[-1]:
                    t_acc.append(ti)
                    y_acc.append(yi)
    return t_acc, y_acc


def _downsample_full_display(t_list, y_list, max_pts, style):
    if not t_list or max_pts < 2:
        return list(t_list), list(y_list)
    n = len(t_list)
    if n <= max_pts:
        return list(t_list), list(y_list)
    if style == "step" or not USE_ENVELOPE_DOWNSAMPLE:
        return _downsample_full(t_list, y_list, max_pts)
    ta = np.asarray(t_list, dtype=np.float64)
    ya = np.asarray(y_list, dtype=np.float64)
    return _envelope_downsample_tt_yy(ta, ya, max_pts)


def _downsample_full(t_list, y_list, max_pts):
    """Прореживание всего ряда (когда окно по времени ещё не задано)."""
    if not t_list or max_pts < 2:
        return list(t_list), list(y_list)
    n = len(t_list)
    if n <= max_pts:
        return list(t_list), list(y_list)
    step = max(1, n // max_pts)
    ta = np.asarray(t_list, dtype=np.float64)
    ya = np.asarray(y_list, dtype=np.float64)
    idx = np.arange(0, n, step, dtype=np.intp)
    return ta[idx].tolist(), ya[idx].tolist()


def _align_y_to_t_len(y_list, nt):
    """Длина y == nt (дополнение nan справа при необходимости)."""
    n = len(y_list)
    if n >= nt:
        return y_list[:nt]
    return y_list + [float("nan")] * (nt - n)


def _uniform_downsample_arrays(tt, yy, max_pts):
    """tt, yy — numpy 1d одинаковой длины; уже срез по окну времени."""
    n = int(len(tt))
    if n == 0 or max_pts < 2:
        return [], []
    if n <= max_pts:
        return tt.tolist(), yy.tolist()
    step = max(1, n // max_pts)
    idx = np.arange(0, n, step, dtype=np.intp)
    return tt[idx].tolist(), yy[idx].tolist()


def build_curve_payloads(graphs, win, artists_dict):
    """
    Для каждой линии, у которой есть артист в artists_dict, вычисляет прореженные (t, y) для отрисовки.
    Итерирует (key, tx, yy, style), key = (ied, gname, lname).
    """
    for (ied, gname), g in graphs.items():
        t_list = g["t"]
        for lname, ld in g["lines"].items():
            key = (ied, gname, lname)
            if key not in artists_dict:
                continue
            style = ld.get("style", "line")
            if not t_list:
                yield key, [], [], style
                continue
            t_np = np.asarray(t_list, dtype=np.float64)
            max_pts = _effective_display_max_points(t_list, win)
            if win is None:
                y_list = _align_y_to_t_len(ld["y"], len(t_list))
                tx, yy = _downsample_full_display(t_list, y_list, max_pts, style)
                if style == "step" and tx and yy:
                    tx, yy = _compress_step_plateau(tx, yy)
                yield key, tx, yy, style
            else:
                tlo, thi = float(win[0]), float(win[1])
                i0 = int(np.searchsorted(t_np, tlo, side="left"))
                i1 = int(np.searchsorted(t_np, thi, side="right"))
                if i1 <= i0:
                    yield key, [], [], style
                    continue
                tt = t_np[i0:i1]
                nt_full = len(t_list)
                y_list = _align_y_to_t_len(ld["y"], nt_full)
                yy = np.asarray(y_list, dtype=np.float64)[i0:i1]
                if len(yy) == 0:
                    yield key, [], [], style
                    continue
                if style == "step" or not USE_ENVELOPE_DOWNSAMPLE:
                    tx, yy2 = _uniform_downsample_arrays(tt, yy, max_pts)
                else:
                    tx, yy2 = _envelope_downsample_tt_yy(tt, yy, max_pts)
                if style == "step" and tx and yy2:
                    tx, yy2 = _compress_step_plateau(tx, yy2)
                yield key, tx, yy2, style


class TelemetryPlotter:
    def __init__(self):
        # (ied, gname) -> {"t": [...], "lines": {lname: {"y": [...], "style": str}}, "_np_ver": int}
        self._graphs = {}
        self._gname_order = []
        self._fig = plt.figure(figsize=(14, 9))
        self._fig.canvas.manager.set_window_title("IED telemetry")
        self._fig.canvas.mpl_connect("close_event", lambda _ev: _terminate_process())
        self._fig.canvas.mpl_connect("key_press_event", self._on_key_press)
        self._fig.canvas.mpl_connect("scroll_event", self._on_scroll)
        self._axes = {}
        self._artists = {}
        self._layout_key = None
        self._last_draw_t = 0.0
        self._last_relim_t = 0.0
        self._sim_done = False
        self._xlim_manual = None
        self._t_min_seen = None
        self._t_max_seen = None
        self._span_cache = None
        self._control_widgets = []
        self._range_slider = None
        self._btn_full = None
        self._suppress_range_slider = False
        self._dirty_data = True
        self._dirty_view = True
        self._axes_pending_legend = set()

    def _invalidate_span_cache(self):
        self._span_cache = None

    def _update_t_bounds(self, t):
        if self._t_min_seen is None:
            self._t_min_seen = self._t_max_seen = t
        else:
            self._t_min_seen = min(self._t_min_seen, t)
            self._t_max_seen = max(self._t_max_seen, t)
        self._invalidate_span_cache()

    def _recompute_seen_t_bounds(self):
        self._t_min_seen = None
        self._t_max_seen = None
        for g in self._graphs.values():
            t = g["t"]
            if not t:
                continue
            t0 = float(t[0])
            t1 = float(t[-1])
            if self._t_min_seen is None:
                self._t_min_seen = self._t_max_seen = t0
            self._t_min_seen = min(self._t_min_seen, t0)
            self._t_max_seen = max(self._t_max_seen, t1)
        self._invalidate_span_cache()

    def _data_span_core(self):
        if self._span_cache is not None:
            return self._span_cache
        if self._t_min_seen is None or self._t_max_seen is None:
            return None
        lo, hi = self._t_min_seen, self._t_max_seen
        if hi <= lo:
            hi = lo + 1e-12
        self._span_cache = (lo, hi)
        return self._span_cache

    def _data_t_range(self):
        span = self._data_span_core()
        if span is None:
            return None
        lo, hi = span
        pad = (hi - lo) * 0.02 + 1e-12
        return lo - pad, hi + pad

    def _register_graph(self, _ied, gname):
        if gname not in self._gname_order:
            self._gname_order.append(gname)

    def _layout_fingerprint(self):
        ieds = sorted({k[0] for k in self._graphs.keys()})
        col_ieds = ieds[:2] if len(ieds) >= 2 else ieds
        return (tuple(self._gname_order), tuple(col_ieds))

    def _destroy_time_widgets(self):
        for w in self._control_widgets:
            ax = getattr(w, "ax", None)
            if ax is not None:
                try:
                    ax.remove()
                except Exception:
                    pass
        self._control_widgets.clear()
        self._range_slider = None
        self._btn_full = None

    def _abs_from_norm(self, n0, n1):
        span = self._data_span_core()
        if span is None:
            return None
        dlo, dhi = span
        w = dhi - dlo
        n0 = max(0.0, min(1.0, float(n0)))
        n1 = max(0.0, min(1.0, float(n1)))
        if n1 <= n0:
            n0, n1 = 0.0, 1.0
        return dlo + n0 * w, dlo + n1 * w

    def _norm_from_abs(self, lo, hi):
        span = self._data_span_core()
        if span is None:
            return 0.0, 1.0
        dlo, dhi = span
        w = dhi - dlo
        if w <= 0:
            return 0.0, 1.0
        n0 = (lo - dlo) / w
        n1 = (hi - dlo) / w
        return max(0.0, min(1.0, n0)), max(0.0, min(1.0, n1))

    def _set_range_slider_norm(self, n0, n1):
        if self._range_slider is None:
            return
        self._suppress_range_slider = True
        try:
            self._range_slider.set_val((n0, n1))
        finally:
            self._suppress_range_slider = False

    def _create_time_widgets(self):
        self._destroy_time_widgets()
        ax_slider = self._fig.add_axes([0.07, 0.018, 0.68, 0.038])
        ax_btn = self._fig.add_axes([0.78, 0.018, 0.18, 0.038])
        n0, n1 = 0.0, 1.0
        if self._xlim_manual is not None and self._data_span_core() is not None:
            n0, n1 = self._norm_from_abs(self._xlim_manual[0], self._xlim_manual[1])
        self._range_slider = RangeSlider(
            ax_slider,
            "Время ",
            0.0,
            1.0,
            valinit=(n0, n1),
            valstep=1e-7,
        )
        self._range_slider.on_changed(self._on_range_slider_changed)
        self._btn_full = Button(ax_btn, "Весь диапазон")
        self._btn_full.on_clicked(self._on_full_time_range)
        self._control_widgets = [self._range_slider, self._btn_full]

    def _on_range_slider_changed(self, _val):
        if self._suppress_range_slider:
            return
        if self._range_slider is None:
            return
        n0, n1 = self._range_slider.val
        abs_rng = self._abs_from_norm(n0, n1)
        if abs_rng is None:
            return
        lo, hi = abs_rng
        if hi <= lo:
            return
        self._xlim_manual = (lo, hi)
        self._dirty_view = True
        self._sync_all_artists_data()
        self._apply_xlim_all()
        for ax in self._axes.values():
            ax.relim(visible_only=True)
            ax.autoscale_view(scalex=False, scaley=True)
        self._fig.canvas.draw_idle()

    def _rebuild_layout(self):
        gnames = self._gname_order
        if not gnames:
            return
        ieds = sorted({k[0] for k in self._graphs.keys()})
        if not ieds:
            return
        col_ieds = ieds[:2] if len(ieds) >= 2 else ieds
        nrows = len(gnames)
        ncols = len(col_ieds)

        self._destroy_time_widgets()
        self._fig.clf()
        self._fig.canvas.mpl_connect("close_event", lambda _ev: _terminate_process())
        self._fig.canvas.mpl_connect("key_press_event", self._on_key_press)
        self._fig.canvas.mpl_connect("scroll_event", self._on_scroll)
        self._axes.clear()
        self._artists.clear()

        for ri, gname in enumerate(gnames):
            for ci, ied in enumerate(col_ieds):
                pos = ri * ncols + ci + 1
                ax = self._fig.add_subplot(nrows, ncols, pos)
                key = (ied, gname)
                self._axes[key] = ax
                ax.set_title(f"{ied} — {gname}")
                ax.grid(True, alpha=0.3)

        self._fig.tight_layout(rect=(0.0, 0.08, 1.0, 1.0))
        self._recreate_all_artists()
        self._create_time_widgets()
        self._layout_key = self._layout_fingerprint()

    def _recreate_all_artists(self):
        for (ied, gname) in self._axes.keys():
            ax = self._axes[(ied, gname)]
            ax.set_title(f"{ied} — {gname}")
            ax.grid(True, alpha=0.3)
        for (ied, gname), g in self._graphs.items():
            ax = self._axes.get((ied, gname))
            if ax is None:
                continue
            for lname, ld in g["lines"].items():
                key = (ied, gname, lname)
                kw = {"label": lname}
                if ld.get("style") == "step":
                    kw["drawstyle"] = "steps-post"
                (ln,) = ax.plot([], [], **kw)
                self._artists[key] = ln
        for ax in self._axes.values():
            handles, labels = ax.get_legend_handles_labels()
            if handles:
                ax.legend(handles, labels, loc="upper right", fontsize=7)
        self._axes_pending_legend.clear()
        self._dirty_data = True
        self._dirty_view = True
        self._sync_all_artists_data(force=True)
        for ax in self._axes.values():
            ax.relim(visible_only=True)
            ax.autoscale_view(scalex=False, scaley=True)
        self._apply_xlim_all()

    def _ensure_graph(self, ied, gname):
        k = (ied, gname)
        if k not in self._graphs:
            self._graphs[k] = {"t": [], "lines": {}, "_np_ver": 0}
        return self._graphs[k]

    def _ensure_line_bucket(self, ied, gname, lname, style):
        g = self._ensure_graph(ied, gname)
        if lname not in g["lines"]:
            nt = len(g["t"])
            g["lines"][lname] = {"y": [float("nan")] * nt, "style": style}
        else:
            g["lines"][lname]["style"] = style
        return g["lines"][lname]

    def _append_graph_sample(self, ied, gname, t, line_updates):
        """line_updates: list of (lname, value, style). Один общий t на граф."""
        g = self._ensure_graph(ied, gname)
        g["t"].append(t)
        g["_np_ver"] = int(g.get("_np_ver", 0)) + 1
        L = len(g["t"])
        for lname, yval, style in line_updates:
            lb = self._ensure_line_bucket(ied, gname, lname, style)
            y = lb["y"]
            while len(y) < L - 1:
                y.append(float("nan"))
            if len(y) == L - 1:
                y.append(float(yval))
            elif len(y) == L:
                y[-1] = float(yval)
            else:
                del y[L:]
                y.append(float(yval))
            lb["style"] = style

    def _ensure_artist(self, key, ied, gname, lname, style):
        if key in self._artists:
            return
        ax = self._axes.get((ied, gname))
        if ax is None:
            return
        kw = {"label": lname}
        if style == "step":
            kw["drawstyle"] = "steps-post"
        (ln,) = ax.plot([], [], **kw)
        self._artists[key] = ln
        self._axes_pending_legend.add((ied, gname))

    def _flush_pending_legends(self):
        for ied, gname in self._axes_pending_legend:
            ax = self._axes.get((ied, gname))
            if ax is None:
                continue
            handles, labels = ax.get_legend_handles_labels()
            if handles:
                ax.legend(handles, labels, loc="upper right", fontsize=7)
        self._axes_pending_legend.clear()

    def _current_x_window(self):
        if self._xlim_manual is not None:
            return self._xlim_manual
        return self._data_t_range()

    def _sync_all_artists_data(self, force=False):
        if not force and not self._dirty_data and not self._dirty_view:
            return
        self._dirty_data = False
        self._dirty_view = False

        win = self._current_x_window()
        for key, tx, yy, _style in build_curve_payloads(self._graphs, win, self._artists):
            ln = self._artists.get(key)
            if ln is not None:
                ln.set_data(tx, yy)

    def _apply_shared_xlim_auto(self):
        rng = self._data_t_range()
        if rng is None:
            return
        lo, hi = rng
        for ax in self._axes.values():
            ax.set_xlim(lo, hi)

    def _apply_xlim_all(self):
        if not self._axes:
            return
        if self._xlim_manual is not None:
            lo, hi = self._xlim_manual
            for ax in self._axes.values():
                ax.set_xlim(lo, hi)
        else:
            self._apply_shared_xlim_auto()

    def _current_time_span(self):
        if self._xlim_manual is not None:
            return self._xlim_manual
        ax0 = next(iter(self._axes.values()), None)
        if ax0 is None:
            return None
        return ax0.get_xlim()

    def _on_full_time_range(self, _event=None):
        self._xlim_manual = None
        self._apply_shared_xlim_auto()
        self._set_range_slider_norm(0.0, 1.0)
        self._dirty_view = True
        self._sync_all_artists_data()
        for ax in self._axes.values():
            ax.relim(visible_only=True)
            ax.autoscale_view(scalex=False, scaley=True)
        self._fig.canvas.draw_idle()

    def _on_key_press(self, event):
        if not self._axes or event.key is None:
            return
        key = str(event.key).lower()
        if key in ("h", "home"):
            self._on_full_time_range()
            return
        span = self._current_time_span()
        if span is None:
            return
        lo, hi = span
        width = hi - lo
        if width <= 0:
            return
        if key in ("+", "plus", "="):
            cx = 0.5 * (lo + hi)
            nw = width / TIME_ZOOM_FACTOR
            self._xlim_manual = (cx - nw * 0.5, cx + nw * 0.5)
        elif key in ("-", "minus", "_"):
            cx = 0.5 * (lo + hi)
            nw = width * TIME_ZOOM_FACTOR
            self._xlim_manual = (cx - nw * 0.5, cx + nw * 0.5)
        elif key == "left":
            sh = width * TIME_PAN_FRACTION
            self._xlim_manual = (lo - sh, hi - sh)
        elif key == "right":
            sh = width * TIME_PAN_FRACTION
            self._xlim_manual = (lo + sh, hi + sh)
        else:
            return
        self._clamp_xlim_to_data()
        self._set_range_slider_norm(*self._norm_from_abs(self._xlim_manual[0], self._xlim_manual[1]))
        self._dirty_view = True
        self._sync_all_artists_data()
        self._apply_xlim_all()
        for ax in self._axes.values():
            ax.relim(visible_only=True)
            ax.autoscale_view(scalex=False, scaley=True)
        self._fig.canvas.draw_idle()

    def _clamp_xlim_to_data(self):
        if self._xlim_manual is None:
            return
        rng = self._data_t_range()
        if rng is None:
            return
        dlo, dhi = rng
        lo, hi = self._xlim_manual
        if hi - lo > dhi - dlo:
            self._xlim_manual = (dlo, dhi)
            return
        if lo < dlo:
            hi += dlo - lo
            lo = dlo
        if hi > dhi:
            lo -= hi - dhi
            hi = dhi
        lo = max(lo, dlo)
        hi = min(hi, dhi)
        if hi <= lo:
            lo, hi = dlo, dhi
        self._xlim_manual = (lo, hi)

    def _on_scroll(self, event):
        if not self._axes or event.inaxes is None:
            return
        step = getattr(event, "step", None) or 0
        if step == 0:
            return
        span = self._current_time_span()
        if span is None:
            return
        lo, hi = span
        width = hi - lo
        if width <= 0 or event.xdata is None:
            return
        cx = float(event.xdata)
        factor = TIME_ZOOM_FACTOR if step > 0 else 1.0 / TIME_ZOOM_FACTOR
        nw = width / factor
        self._xlim_manual = (cx - (cx - lo) / width * nw, cx + (hi - cx) / width * nw)
        self._clamp_xlim_to_data()
        self._set_range_slider_norm(*self._norm_from_abs(self._xlim_manual[0], self._xlim_manual[1]))
        self._dirty_view = True
        self._sync_all_artists_data()
        self._apply_xlim_all()
        for ax in self._axes.values():
            ax.relim(visible_only=True)
            ax.autoscale_view(scalex=False, scaley=True)
        self._fig.canvas.draw_idle()

    def finalize_after_simulation(self):
        if self._sim_done or not self._graphs:
            return
        self._sim_done = True
        self._dirty_data = True
        self._dirty_view = True
        self._sync_all_artists_data(force=True)
        for ax in self._axes.values():
            ax.relim(visible_only=True)
            ax.autoscale_view(scalex=False, scaley=True)
        self._apply_xlim_all()
        self._fig.canvas.draw_idle()

    def ingest_message(self, msg):
        ied = msg["name"]
        t = float(msg["t"])
        self._update_t_bounds(t)
        for g in msg.get("graphs", []):
            gname = g["graph_name"]
            style = g.get("plot_style", "line")
            self._register_graph(ied, gname)
            line_updates = [(ln["line_name"], float(ln["value"]), style) for ln in g.get("lines", [])]
            if line_updates:
                self._append_graph_sample(ied, gname, t, line_updates)

        fp = self._layout_fingerprint()
        layout_changed = fp != self._layout_key
        if layout_changed:
            self._rebuild_layout()
        if not layout_changed:
            for g in msg.get("graphs", []):
                gname = g["graph_name"]
                style = g.get("plot_style", "line")
                for ln in g.get("lines", []):
                    lname = ln["line_name"]
                    key = (ied, gname, lname)
                    self._ensure_artist(key, ied, gname, lname, style)

        self._dirty_data = True
        _trim_graphs_tail(self._graphs, TELEMETRY_TAIL_MAX_POINTS, TELEMETRY_TRIM_TRIGGER_MULT, self)
        self._flush_pending_legends()

    def refresh_plot(self, force=False):
        if not self._axes:
            return
        now = time.monotonic()
        if not force and (now - self._last_draw_t) < MIN_DRAW_INTERVAL:
            return
        self._last_draw_t = now

        self._sync_all_artists_data(force=force)

        if self._sim_done:
            self._apply_xlim_all()
            self._fig.canvas.draw_idle()
            return

        do_relim = force or (now - self._last_relim_t) >= RELIM_INTERVAL_S
        if do_relim:
            self._last_relim_t = now
            for ax in self._axes.values():
                ax.relim(visible_only=True)
                ax.autoscale_view(scalex=False, scaley=True)
            if self._xlim_manual is None:
                self._apply_shared_xlim_auto()
                self._set_range_slider_norm(0.0, 1.0)
            else:
                self._apply_xlim_all()

        self._fig.canvas.draw_idle()
        try:
            self._fig.canvas.flush_events()
        except Exception:
            pass


def _monitoring_ui_choice():
    raw = os.environ.get("MONITORING_UI", MONITORING_UI_DEFAULT)
    v = (raw or MONITORING_UI_DEFAULT).strip().lower()
    if v in ("matplotlib", "mpl", "0", "false", "no"):
        return "matplotlib"
    if v in ("pyqtgraph", "pg", "1", "true", "yes"):
        try:
            import pyqtgraph  # noqa: F401

            return "pyqtgraph"
        except ImportError:
            print(
                "MONITORING_UI указывает pyqtgraph, но пакет не найден. Установите: pip install pyqtgraph PyQt5  (или PySide6). Используется matplotlib.",
                file=sys.stderr,
            )
            return "matplotlib"
    if v == "auto":
        try:
            import pyqtgraph  # noqa: F401

            return "pyqtgraph"
        except ImportError:
            return "matplotlib"
    return "matplotlib"


def create_telemetry_plotter():
    return TelemetryPlotterPg() if _monitoring_ui_choice() == "pyqtgraph" else TelemetryPlotter()


class TelemetryPlotterPg(TelemetryPlotter):
    """
    Те же ряды и ingest, что у TelemetryPlotter; отрисовка PyQtGraph (GPU/OpenGL, быстрый setData).
    Окно R–X по-прежнему matplotlib (RxPlanePlotter).
    """

    def __init__(self):
        import pyqtgraph as pg
        from pyqtgraph.Qt import QtGui, QtWidgets

        self._graphs = {}
        self._gname_order = []
        self._layout_key = None
        self._last_draw_t = 0.0
        self._last_relim_t = 0.0
        self._sim_done = False
        self._xlim_manual = None
        self._t_min_seen = None
        self._t_max_seen = None
        self._span_cache = None
        self._dirty_data = True
        self._dirty_view = True
        self._plots = {}
        self._axes = self._plots
        self._curves = {}
        self._artists = self._curves
        self._control_widgets = []
        self._range_slider = None
        self._btn_full = None
        self._suppress_range_slider = False
        self._axes_pending_legend = set()
        self._time_region = None
        self._suppress_region = False
        self._first_plot_item = None
        self._pg = pg

        if QtWidgets.QApplication.instance() is None:
            pg.mkQApp("IED telemetry")
        pg.setConfigOptions(antialias=False, foreground="k", background="w")
        self._glw = pg.GraphicsLayoutWidget(show=True)
        self._glw.setWindowTitle("IED telemetry (pyqtgraph)")
        self._glw.resize(1400, 900)
        self._glw.show()

        def _close_ev(ev):
            _terminate_process()
            ev.accept()

        self._glw.closeEvent = _close_ev  # type: ignore[method-assign]

        sc = QtGui.QShortcut(QtGui.QKeySequence("H"), self._glw)
        sc.activated.connect(self._on_full_time_range)

    def _destroy_time_widgets(self):
        self._control_widgets.clear()
        self._range_slider = None
        self._btn_full = None

    def _create_time_widgets(self):
        pass

    def _set_range_slider_norm(self, n0, n1):
        if self._time_region is None or self._data_span_core() is None:
            return
        dlo, dhi = self._data_span_core()
        w = dhi - dlo
        if w <= 0:
            return
        n0 = max(0.0, min(1.0, float(n0)))
        n1 = max(0.0, min(1.0, float(n1)))
        if n1 <= n0:
            n0, n1 = 0.0, 1.0
        lo, hi = dlo + n0 * w, dlo + n1 * w
        self._suppress_region = True
        try:
            self._time_region.setRegion((lo, hi))
        finally:
            self._suppress_region = False

    def _on_range_slider_changed(self, _val=None):
        pass

    def _on_time_region_pg(self):
        if self._suppress_region or self._time_region is None:
            return
        lo, hi = self._time_region.getRegion()
        if hi <= lo:
            return
        self._xlim_manual = (float(lo), float(hi))
        self._dirty_view = True
        self._sync_all_artists_data()
        for pi in self._plots.values():
            pi.setXRange(lo, hi, padding=0)

    def _rebuild_layout(self):
        gnames = self._gname_order
        if not gnames:
            return
        ieds = sorted({k[0] for k in self._graphs.keys()})
        if not ieds:
            return
        col_ieds = ieds[:2] if len(ieds) >= 2 else ieds
        nrows = len(gnames)
        ncols = len(col_ieds)

        self._destroy_time_widgets()
        self._glw.clear()
        self._plots.clear()
        self._curves.clear()
        self._time_region = None
        self._first_plot_item = None

        for ri, gname in enumerate(gnames):
            for ci, ied in enumerate(col_ieds):
                pi = self._glw.addPlot(row=ri, col=ci, title=f"{ied} — {gname}")
                pi.showGrid(x=True, y=True, alpha=0.35)
                pi.addLegend(offset=(10, 10))
                key = (ied, gname)
                self._plots[key] = pi
                if self._first_plot_item is None:
                    self._first_plot_item = pi
                else:
                    pi.setXLink(self._first_plot_item)

        self._glw.nextRow()
        from pyqtgraph.Qt import QtWidgets

        btn = QtWidgets.QPushButton("Весь диапазон")
        btn.clicked.connect(self._on_full_time_range)
        proxy = QtWidgets.QGraphicsProxyWidget()
        proxy.setWidget(btn)
        self._glw.addItem(proxy, row=nrows, col=0, colspan=ncols)

        self._recreate_all_artists()
        self._layout_key = self._layout_fingerprint()
        span = self._data_span_core()
        if span and self._first_plot_item is not None:
            dlo, dhi = span
            self._time_region = self._pg.LinearRegionItem(values=(dlo, dhi), movable=True)
            self._first_plot_item.addItem(self._time_region)
            self._time_region.sigRegionChanged.connect(self._on_time_region_pg)

    def _recreate_all_artists(self):
        for pi in self._plots.values():
            pi.clear()
            pi.addLegend(offset=(10, 10))
        self._curves.clear()
        for (ied, gname), g in self._graphs.items():
            pi = self._plots.get((ied, gname))
            if pi is None:
                continue
            for lname, ld in g["lines"].items():
                key = (ied, gname, lname)
                pen = self._pg.mkPen(self._pg.intColor(abs(hash(lname)) % 256), width=1.3)
                cr = pi.plot([], [], pen=pen, name=lname, connect="finite")
                self._curves[key] = cr
        self._axes_pending_legend.clear()
        self._dirty_data = True
        self._dirty_view = True
        self._sync_all_artists_data(force=True)
        for pi in self._plots.values():
            pi.enableAutoRange(axis=self._pg.ViewBox.YAxis, enable=True)
        self._apply_xlim_all()

    def _ensure_artist(self, key, ied, gname, lname, style):
        if key in self._curves:
            return
        pi = self._plots.get((ied, gname))
        if pi is None:
            return
        pen = self._pg.mkPen(self._pg.intColor(abs(hash(lname)) % 256), width=1.3)
        cr = pi.plot([], [], pen=pen, name=lname, connect="finite")
        self._curves[key] = cr

    def _flush_pending_legends(self):
        self._axes_pending_legend.clear()

    def _apply_shared_xlim_auto(self):
        rng = self._data_t_range()
        if rng is None:
            return
        lo, hi = rng
        self._suppress_region = True
        try:
            if self._time_region is not None:
                self._time_region.setBounds((lo, hi))
                self._time_region.setRegion((lo, hi))
        finally:
            self._suppress_region = False
        for pi in self._plots.values():
            pi.setXRange(lo, hi, padding=0)

    def _apply_xlim_all(self):
        if not self._plots:
            return
        if self._xlim_manual is not None:
            lo, hi = self._xlim_manual
            self._suppress_region = True
            try:
                if self._time_region is not None:
                    self._time_region.setRegion((lo, hi))
            finally:
                self._suppress_region = False
            for pi in self._plots.values():
                pi.setXRange(lo, hi, padding=0)
        else:
            self._apply_shared_xlim_auto()

    def _current_time_span(self):
        if self._xlim_manual is not None:
            return self._xlim_manual
        if not self._plots:
            return None
        pi0 = next(iter(self._plots.values()))
        xr = pi0.vb.viewRange()[0]
        return float(xr[0]), float(xr[1])

    def _on_full_time_range(self, _event=None):
        self._xlim_manual = None
        self._apply_shared_xlim_auto()
        self._set_range_slider_norm(0.0, 1.0)
        self._dirty_view = True
        self._sync_all_artists_data()
        for pi in self._plots.values():
            pi.enableAutoRange(axis=self._pg.ViewBox.YAxis, enable=True)

    def _on_key_press(self, event):
        pass

    def _on_scroll(self, event):
        pass

    def _sync_all_artists_data(self, force=False):
        if not force and not self._dirty_data and not self._dirty_view:
            return
        self._dirty_data = False
        self._dirty_view = False
        win = self._current_x_window()
        span = self._data_span_core()
        if span and self._time_region is not None:
            dlo, dhi = span
            self._suppress_region = True
            try:
                self._time_region.setBounds((dlo, dhi))
            finally:
                self._suppress_region = False
        for key, tx, yy, _style in build_curve_payloads(self._graphs, win, self._curves):
            cr = self._curves.get(key)
            if cr is None:
                continue
            if not tx:
                cr.setData([], [])
                continue
            cr.setData(np.asarray(tx, dtype=np.float64), np.asarray(yy, dtype=np.float64), connect="finite")

    def finalize_after_simulation(self):
        if self._sim_done or not self._graphs:
            return
        self._sim_done = True
        self._dirty_data = True
        self._dirty_view = True
        self._sync_all_artists_data(force=True)
        for pi in self._plots.values():
            pi.enableAutoRange(axis=self._pg.ViewBox.YAxis, enable=True)
        self._apply_xlim_all()
        from pyqtgraph.Qt import QtWidgets

        QtWidgets.QApplication.processEvents()

    def refresh_plot(self, force=False):
        if not self._plots:
            return
        now = time.monotonic()
        if not force and (now - self._last_draw_t) < MIN_DRAW_INTERVAL:
            return
        self._last_draw_t = now
        self._sync_all_artists_data(force=force)
        from pyqtgraph.Qt import QtWidgets

        if self._sim_done:
            self._apply_xlim_all()
            QtWidgets.QApplication.processEvents()
            return
        do_relim = force or (now - self._last_relim_t) >= RELIM_INTERVAL_S
        if do_relim:
            self._last_relim_t = now
            for pi in self._plots.values():
                pi.enableAutoRange(axis=self._pg.ViewBox.YAxis, enable=True)
            if self._xlim_manual is None:
                self._apply_shared_xlim_auto()
                self._set_range_slider_norm(0.0, 1.0)
            else:
                self._apply_xlim_all()
        QtWidgets.QApplication.processEvents()


def _rx_three_traces(rx):
    """Три межфазных сопротивления: полные traj из сообщения (legacy) или только точки pr/px."""
    keys = ("z_ab", "z_bc", "z_ca")
    out = []
    for k in keys:
        d = rx.get(k)
        if isinstance(d, dict):
            tr = d.get("traj_r") or []
            tx = d.get("traj_x") or []
            pr = float(d.get("point_r", d.get("pr", 0.0)))
            px = float(d.get("point_x", d.get("px", 0.0)))
            out.append((list(tr), list(tx), pr, px))
    if len(out) == 3:
        return out
    tr = list(rx.get("traj_r") or [])
    tx = list(rx.get("traj_x") or [])
    pr = float(rx.get("point_r", rx.get("pr", 0.0)))
    px = float(rx.get("point_x", rx.get("px", 0.0)))
    return [(tr, tx, pr, px), ([], [], 0.0, 0.0), ([], [], 0.0, 0.0)]


class RxPlanePlotter:
    """Плоскость R–X: зоны PDIS один раз с сервера; траектории накапливаются из pr/px по шагам."""

    _pick_colors = ("#e41a1c", "#377eb8", "#4daf4a")
    _ret_colors = ("#ff9896", "#aec7e8", "#98df8a")
    _traj_colors = ("#1f78b4", "#33a02c", "#e31a1c")
    _traj_labels = ("Z_ab", "Z_bc", "Z_ca")

    def __init__(self):
        self._zones_sig = None
        self._cached_zones = None
        self._rx_ied = None
        self._last_mx = 0
        self._traj_lines = []
        self._point_scat = None
        self._cum_traces = [([], []), ([], []), ([], [])]
        self._rx_update_fp = None
        self._fig = plt.figure(figsize=(8, 8))
        self._fig.canvas.manager.set_window_title("IED R–X (Z_ab, Z_bc, Z_ca)")
        self._fig.canvas.mpl_connect("close_event", lambda _ev: _terminate_process())
        self._ax = self._fig.add_subplot(111)
        self._ax.set_xlabel("R, Ohm")
        self._ax.set_ylabel("X, Ohm")
        self._ax.axhline(0.0, color="k", linewidth=0.6, alpha=0.5)
        self._ax.axvline(0.0, color="k", linewidth=0.6, alpha=0.5)
        self._ax.grid(True, alpha=0.3)
        self._ax.set_aspect("auto")

    def _draw_zones(self, ax, zones):
        for i, z in enumerate(zones):
            pick = z.get("pickup") or []
            ret = z.get("return_poly") or []
            c_p = self._pick_colors[i % len(self._pick_colors)]
            c_r = self._ret_colors[i % len(self._ret_colors)]
            if len(ret) >= 3:
                xs = [p[0] for p in ret]
                ys = [p[1] for p in ret]
                ax.plot(xs, ys, "--", color=c_r, linewidth=1.2)
            if len(pick) >= 3:
                xs = [p[0] for p in pick]
                ys = [p[1] for p in pick]
                ax.fill(xs, ys, facecolor=c_p, alpha=0.15, edgecolor=c_p, linewidth=1.4, linestyle="-")

    def ingest_rx_step(self, ied_name, rx):
        """Принять один шаг rx_view (точки pr/px; зоны — в первом сообщении сессии)."""
        if not isinstance(rx, dict):
            return
        if ied_name != self._rx_ied:
            self._rx_ied = ied_name
            self._cum_traces = [([], []), ([], []), ([], [])]
            self._zones_sig = None
            self._cached_zones = None
            self._traj_lines = []
            self._point_scat = None
            self._rx_update_fp = None
        self._last_mx = int(rx.get("max_zone_revision", 0))
        raw_zones = rx.get("zones")
        if raw_zones:
            self._cached_zones = raw_zones
        keys = ("z_ab", "z_bc", "z_ca")
        for i, k in enumerate(keys):
            d = rx.get(k)
            if not isinstance(d, dict):
                d = {}
            tr_legacy = d.get("traj_r")
            tx_legacy = d.get("traj_x")
            if (
                isinstance(tr_legacy, list)
                and isinstance(tx_legacy, list)
                and len(tr_legacy) == len(tx_legacy)
                and tr_legacy
            ):
                self._cum_traces[i] = (list(tr_legacy), list(tx_legacy))
                continue
            pr = float(d.get("point_r", d.get("pr", 0.0)))
            px = float(d.get("point_x", d.get("px", 0.0)))
            tr, tx = self._cum_traces[i]
            tr.append(pr)
            tx.append(px)
            lim = RX_TRAJ_MAX_STORE_POINTS
            if lim > 0 and len(tr) > lim:
                cut = len(tr) - lim
                del tr[:cut]
                del tx[:cut]

    def redraw(self):
        """Перерисовать по накопленным траекториям."""
        ax = self._ax
        mx = self._last_mx
        ied_name = self._rx_ied or ""
        if self._cached_zones:
            sig = (mx, tuple((z.get("name"), int(z.get("revision", 0))) for z in self._cached_zones))
        elif self._zones_sig is not None:
            sig = self._zones_sig
        else:
            sig = (mx, ())
        merged = []
        for i in range(3):
            tr0, tx0 = self._cum_traces[i]
            pr = float(tr0[-1]) if tr0 else 0.0
            px = float(tx0[-1]) if tx0 else 0.0
            cr, cx = list(tr0), list(tx0)
            if RX_TRAJ_LEAD_SUPPRESS_OHM > 0.0:
                cr, cx = _rx_strip_leading_near_origin(cr, cx, RX_TRAJ_LEAD_SUPPRESS_OHM)
            if RX_TRAJ_SEGMENT_BREAK_OHM > 0.0:
                cr, cx = _rx_break_xy_jumps(cr, cx, RX_TRAJ_SEGMENT_BREAK_OHM)
            merged.append((cr, cx, pr, px))
        traces = merged

        prs = [t[2] for t in traces]
        pxs = [t[3] for t in traces]
        traj_fp = tuple(
            (len(tr), float(tr[-1]), float(tx[-1]))
            if tr and tx and len(tr) == len(tx)
            else (0, 0.0, 0.0)
            for tr, tx, _, __ in traces
        )
        fp = (traj_fp, tuple(round(float(p), 9) for p in prs + pxs))

        need_zones = self._zones_sig != sig or len(self._traj_lines) != 3 or self._point_scat is None
        if not need_zones and fp == self._rx_update_fp:
            return
        self._rx_update_fp = fp

        if need_zones:
            self._zones_sig = sig
            ax.clear()
            ax.set_xlabel("R, Ohm")
            ax.set_ylabel("X, Ohm")
            ax.axhline(0.0, color="k", linewidth=0.6, alpha=0.5)
            ax.axvline(0.0, color="k", linewidth=0.6, alpha=0.5)
            ax.grid(True, alpha=0.3)
            ax.set_aspect("auto")
            self._draw_zones(ax, self._cached_zones or [])
            self._traj_lines = []
            for i, (tr, tx, _pr, _px) in enumerate(traces):
                c = self._traj_colors[i]
                lab = self._traj_labels[i]
                if len(tr) == len(tx) and len(tr) >= 1:
                    (ln,) = ax.plot(tr, tx, linestyle="--", color=c, linewidth=1.2, alpha=0.9, label=lab)
                else:
                    (ln,) = ax.plot([], [], linestyle="--", color=c, linewidth=1.2, alpha=0.9, label=lab)
                self._traj_lines.append(ln)
            prs = [t[2] for t in traces]
            pxs = [t[3] for t in traces]
            self._point_scat = ax.scatter(
                prs,
                pxs,
                s=[44, 44, 44],
                c=list(self._traj_colors),
                edgecolors="k",
                linewidths=0.7,
                zorder=6,
            )
            ax.set_title(f"{ied_name} — Z_ab, Z_bc, Z_ca (rev {mx})")
            if self._traj_lines:
                ax.legend(self._traj_lines, list(self._traj_labels), loc="upper right", fontsize=8)
            ax.relim(visible_only=True)
            ax.autoscale_view()
        else:
            for ln, (tr, tx, _, __) in zip(self._traj_lines, traces):
                if len(tr) == len(tx):
                    ln.set_data(tr, tx)
            prs = [t[2] for t in traces]
            pxs = [t[3] for t in traces]
            self._point_scat.set_offsets(np.column_stack([prs, pxs]))
            ax.set_title(f"{ied_name} — Z_ab, Z_bc, Z_ca (rev {mx})")

        self._fig.canvas.draw_idle()
        try:
            self._fig.canvas.flush_events()
        except Exception:
            pass

    def update(self, ied_name, rx):
        self.ingest_rx_step(ied_name, rx)
        self.redraw()


def _recv_telemetry_burst(sock, max_messages=TELEMETRY_RECV_DRAIN_CAP):
    """
    Один блокирующий recv (с RCVTIMEO сокета), затем вычитать всю оставшуюся очередь без ожидания.
    Обработчик после этого делает одну перерисовку — все накопленные точки сразу (масштаб по времени ручной не трогаем).
    """
    out = [sock.recv()]
    while len(out) < max_messages:
        try:
            out.append(sock.recv(zmq.NOBLOCK))
        except zmq.Again:
            break
    return out


def _telemetry_main_window_alive(plotter):
    glw = getattr(plotter, "_glw", None)
    if glw is not None:
        try:
            return bool(glw.isVisible() or glw.isMinimized())
        except Exception:
            return False
    fig = getattr(plotter, "_fig", None)
    if fig is None:
        return False
    return plt.fignum_exists(fig.number)


def _telemetry_flush_events(plotter):
    fig = getattr(plotter, "_fig", None)
    if fig is None:
        return
    try:
        fig.canvas.flush_events()
    except Exception:
        pass


def _telemetry_pump_gui(plotter):
    """Пропустить события GUI (matplotlib и при необходимости Qt/pyqtgraph)."""
    _telemetry_flush_events(plotter)
    try:
        from pyqtgraph.Qt import QtWidgets

        app = QtWidgets.QApplication.instance()
        if app is not None:
            app.processEvents()
    except Exception:
        pass


def main():
    plotter = create_telemetry_plotter()
    rx_plane = None
    plt.ion()
    plt.show(block=False)

    idle_cycles = 0
    last_rx_draw = 0.0
    missing_main = 0
    missing_rx = 0
    sim_stopped = False

    while True:
        if not _telemetry_main_window_alive(plotter):
            missing_main += 1
            if missing_main >= FIG_WINDOW_MISSING_DEBOUNCE:
                _terminate_process()
        else:
            missing_main = 0

        if rx_plane is not None:
            if not plt.fignum_exists(rx_plane._fig.number):
                missing_rx += 1
                if missing_rx >= FIG_WINDOW_MISSING_DEBOUNCE:
                    _terminate_process()
            else:
                missing_rx = 0

        try:
            batch = _recv_telemetry_burst(sock)
            idle_cycles = 0
        except zmq.Again:
            if not sim_stopped:
                idle_cycles += 1
                if idle_cycles >= IDLE_CYCLES_FOR_DONE:
                    plotter.finalize_after_simulation()
                    plotter.refresh_plot(force=True)
                    if rx_plane is not None:
                        rx_plane._fig.canvas.draw_idle()
                    time.sleep(0.05)
                    sim_stopped = True
                    idle_cycles = 0
            plotter.refresh_plot(force=False)
            time.sleep(0.05 if sim_stopped else 0.02)
            continue

        playback_delay = _playback_delay_sec()
        anim_every = TELEMETRY_ANIM_UI_EVERY_N
        pending_anim = 0
        for raw in batch:
            try:
                msg = json.loads(raw.decode("utf-8"))
            except (json.JSONDecodeError, UnicodeDecodeError) as err:
                print("Bad JSON:", err, file=sys.stderr)
                continue
            plotter.ingest_message(msg)
            rv = msg.get("rx_view")
            if rv is not None:
                iedn = msg.get("name", "")
                latest_rx_ied = iedn
                if rx_plane is None:
                    rx_plane = RxPlanePlotter()
                rx_plane.ingest_rx_step(iedn, rv)
            if playback_delay > 0.0:
                time.sleep(playback_delay)
            pending_anim += 1
            if pending_anim >= anim_every:
                pending_anim = 0
                plotter.refresh_plot(force=False)
                if rx_plane is not None:
                    rx_plane.redraw()
                    last_rx_draw = time.monotonic()
                _telemetry_pump_gui(plotter)
                if rx_plane is not None:
                    try:
                        rx_plane._fig.canvas.flush_events()
                    except Exception:
                        pass

        plotter.refresh_plot(force=True)
        now = time.monotonic()
        if rx_plane is not None and (now - last_rx_draw) >= RX_UPDATE_MIN_INTERVAL:
            rx_plane.redraw()
            last_rx_draw = now

        try:
            _telemetry_pump_gui(plotter)
            if rx_plane is not None:
                rx_plane._fig.canvas.flush_events()
        except Exception:
            pass


if __name__ == "__main__":
    main()
