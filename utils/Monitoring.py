"""
Live telemetry from C++ IEDs over ZMQ inproc (PULL here, PUSH in C++).

Данные по каждому ряду хранятся полностью; на график попадает не больше
MAX_DISPLAY_POINTS_PER_LINE точек в текущем окне по времени (прореживание).

Ось времени: слайдер «Время» внизу (две ручки — левая/правая граница окна
в долях от полного интервала данных). Кнопка «Весь диапазон» или клавиша h.

Переменные окружения:
- IED_TIME_T0 / IED_TIME_T1 (с) — начальное окно по времени;
- IED_TELEMETRY_ONLY — имя одного IED (например DPP_1): принимать и рисовать только его,
  второй поток данных с сокета по-прежнему читается, но не отображается.

Closing the matplotlib window terminates the whole process.
"""
import bisect
import json
import os
import sys
import time

import zmq

try:
    import matplotlib

    matplotlib.use("TkAgg")
    import matplotlib.pyplot as plt
    from matplotlib.widgets import Button, RangeSlider
except Exception as exc:  # pragma: no cover
    print("matplotlib / TkAgg:", exc, file=sys.stderr)
    raise

try:
    import numpy as np
except ImportError:
    np = None

ctx_addr = zmq_context_address
ctx = zmq.Context.shadow(ctx_addr)

ENDPOINT = "inproc://ied_telemetry"

# Имя одного IED для отображения (пусто = все). Пример: export IED_TELEMETRY_ONLY=DPP_1
TELEMETRY_ONLY_IED = os.environ.get("IED_TELEMETRY_ONLY", "DPP_1").strip()

MIN_DRAW_INTERVAL = 0.05
IDLE_CYCLES_FOR_DONE = 80
RELIM_INTERVAL_S = 0.35
# Сколько точек максимум отдавать в Line2D на один ряд (полные данные в списках).
MAX_DISPLAY_POINTS_PER_LINE = 500

TIME_PAN_FRACTION = 0.1
TIME_ZOOM_FACTOR = 1.25

sock = ctx.socket(zmq.PULL)
sock.bind(ENDPOINT)
sock.setsockopt(zmq.RCVTIMEO, 50)

if "notify_telemetry_pull_bound" in globals():
    notify_telemetry_pull_bound()


def _terminate_process():
    os._exit(0)


def _parse_float(s):
    return float(s.strip().replace(",", "."))


def _points_in_window(t_list, y_list, tlo, thi, max_pts):
    """Вырезка [tlo, thi] и прореживание до max_pts (t не убывает)."""
    if not t_list or max_pts < 2:
        return [], []
    if np is not None:
        ta = np.asarray(t_list, dtype=float)
        ya = np.asarray(y_list, dtype=float)
        i0 = int(np.searchsorted(ta, tlo, side="left"))
        i1 = int(np.searchsorted(ta, thi, side="right"))
        if i1 <= i0:
            return [], []
        tt = ta[i0:i1]
        yy = ya[i0:i1]
        n = len(tt)
        if n <= max_pts:
            return tt.tolist(), yy.tolist()
        step = max(1, n // max_pts)
        idx = np.arange(0, n, step, dtype=np.intp)
        return tt[idx].tolist(), yy[idx].tolist()
    i0 = bisect.bisect_left(t_list, tlo)
    i1 = bisect.bisect_right(t_list, thi)
    if i1 <= i0:
        return [], []
    tt = t_list[i0:i1]
    yy = y_list[i0:i1]
    n = len(tt)
    if n <= max_pts:
        return list(tt), list(yy)
    step = max(1, n // max_pts)
    return tt[::step], yy[::step]


class TelemetryPlotter:
    def __init__(self):
        self._series = {}
        self._gname_order = []
        self._fig = plt.figure(figsize=(14, 9))
        title = "IED telemetry"
        if TELEMETRY_ONLY_IED:
            title = f"IED telemetry — только {TELEMETRY_ONLY_IED}"
        self._fig.canvas.manager.set_window_title(title)
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
        self._control_widgets = []
        self._range_slider = None
        self._btn_full = None
        self._suppress_range_slider = False

        env_lo = os.environ.get("IED_TIME_T0", "").strip()
        env_hi = os.environ.get("IED_TIME_T1", "").strip()
        if env_lo and env_hi:
            try:
                lo, hi = _parse_float(env_lo), _parse_float(env_hi)
                if hi > lo:
                    self._xlim_manual = (lo, hi)
            except ValueError:
                pass

    def _update_t_bounds(self, t):
        if self._t_min_seen is None:
            self._t_min_seen = self._t_max_seen = t
        else:
            self._t_min_seen = min(self._t_min_seen, t)
            self._t_max_seen = max(self._t_max_seen, t)

    def _data_span_core(self):
        if self._t_min_seen is None or self._t_max_seen is None:
            return None
        lo, hi = self._t_min_seen, self._t_max_seen
        if hi <= lo:
            hi = lo + 1e-12
        return lo, hi

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
        ieds = sorted({k[0] for k in self._series.keys()})
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
        ieds = sorted({k[0] for k in self._series.keys()})
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
        for key, data in self._series.items():
            ied, gname, lname = key
            ax = self._axes.get((ied, gname))
            if ax is None:
                continue
            kw = {"label": lname}
            if data.get("style") == "step":
                kw["drawstyle"] = "steps-post"
            (ln,) = ax.plot([], [], **kw)
            self._artists[key] = ln
        for ax in self._axes.values():
            handles, labels = ax.get_legend_handles_labels()
            if handles:
                ax.legend(handles, labels, loc="upper right", fontsize=7)
        self._sync_all_artists_data()
        for ax in self._axes.values():
            ax.relim(visible_only=True)
            ax.autoscale_view(scalex=False, scaley=True)
        self._apply_xlim_all()

    def _ensure_series(self, key, style):
        if key not in self._series:
            self._series[key] = {"t": [], "y": [], "style": style}

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
        handles, labels = ax.get_legend_handles_labels()
        ax.legend(handles, labels, loc="upper right", fontsize=7)

    def _current_x_window(self):
        if self._xlim_manual is not None:
            return self._xlim_manual
        return self._data_t_range()

    def _sync_all_artists_data(self):
        win = self._current_x_window()
        if win is None:
            tlo, thi = 0.0, 1.0
        else:
            tlo, thi = win
        for key, data in self._series.items():
            ln = self._artists.get(key)
            if ln is None:
                continue
            if not data["t"]:
                ln.set_data([], [])
                continue
            tx, yy = _points_in_window(
                data["t"], data["y"], tlo, thi, MAX_DISPLAY_POINTS_PER_LINE
            )
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
        self._sync_all_artists_data()
        self._apply_xlim_all()
        for ax in self._axes.values():
            ax.relim(visible_only=True)
            ax.autoscale_view(scalex=False, scaley=True)
        self._fig.canvas.draw_idle()

    def finalize_after_simulation(self):
        if self._sim_done or not self._series:
            return
        self._sim_done = True
        self._sync_all_artists_data()
        for ax in self._axes.values():
            ax.relim(visible_only=True)
            ax.autoscale_view(scalex=False, scaley=True)
        self._apply_xlim_all()
        self._fig.canvas.draw_idle()

    def ingest_message(self, msg):
        ied = msg["name"]
        if TELEMETRY_ONLY_IED and ied != TELEMETRY_ONLY_IED:
            return
        t = float(msg["t"])
        self._update_t_bounds(t)
        for g in msg.get("graphs", []):
            gname = g["graph_name"]
            style = g.get("plot_style", "line")
            self._register_graph(ied, gname)
            for ln in g.get("lines", []):
                lname = ln["line_name"]
                key = (ied, gname, lname)
                self._ensure_series(key, style)
                self._series[key]["t"].append(t)
                self._series[key]["y"].append(float(ln["value"]))
                self._series[key]["style"] = style

        fp = self._layout_fingerprint()
        if fp != self._layout_key:
            self._rebuild_layout()
            return

        for g in msg.get("graphs", []):
            gname = g["graph_name"]
            style = g.get("plot_style", "line")
            for ln in g.get("lines", []):
                lname = ln["line_name"]
                key = (ied, gname, lname)
                self._ensure_artist(key, ied, gname, lname, style)

    def refresh_plot(self, force=False):
        if not self._axes:
            return
        now = time.monotonic()
        if not force and (now - self._last_draw_t) < MIN_DRAW_INTERVAL:
            return
        self._last_draw_t = now

        self._sync_all_artists_data()

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


def _recv_batch(sock):
    out = [sock.recv()]
    while True:
        try:
            out.append(sock.recv(zmq.NOBLOCK))
        except zmq.Again:
            break
    return out


def main():
    plotter = TelemetryPlotter()
    plt.ion()
    plt.show(block=False)

    idle_cycles = 0

    while True:
        if plotter._fig.number not in plt.get_fignums():
            _terminate_process()

        try:
            batch = _recv_batch(sock)
            idle_cycles = 0
        except zmq.Again:
            idle_cycles += 1
            if idle_cycles >= IDLE_CYCLES_FOR_DONE:
                plotter.finalize_after_simulation()
            plotter.refresh_plot(force=False)
            plt.pause(0.02)
            continue
        for raw in batch:
            try:
                msg = json.loads(raw.decode("utf-8"))
            except (json.JSONDecodeError, UnicodeDecodeError) as err:
                print("Bad JSON:", err, file=sys.stderr)
                continue
            plotter.ingest_message(msg)
        plotter.refresh_plot(force=True)
        plt.pause(0.001)


if __name__ == "__main__":
    main()
