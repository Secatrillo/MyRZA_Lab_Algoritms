"""
Live telemetry from C++ IEDs over ZMQ inproc (PULL here, PUSH in C++).

Layout: two columns — sorted IED names (DFP_1, DFP_2, …), left column first
device, right column second; each row is one graph_name shared by both.

Time axis: full simulation interval on all subplots (shared xlim).

Closing the matplotlib window terminates the whole process (C++ workers stop
with it); without this, main() would return while IED threads keep running.
"""
import json
import os
import sys
import time

import zmq

try:
    import matplotlib
    matplotlib.use("TkAgg")
    import matplotlib.pyplot as plt
except Exception as exc:  # pragma: no cover
    print("matplotlib / TkAgg:", exc, file=sys.stderr)
    raise

ctx_addr = zmq_context_address
ctx = zmq.Context.shadow(ctx_addr)

ENDPOINT = "inproc://ied_telemetry"

# Coalesce ZMQ messages and limit canvas updates when idle (seconds).
MIN_DRAW_INTERVAL = 0.04

sock = ctx.socket(zmq.PULL)
sock.bind(ENDPOINT)
sock.setsockopt(zmq.RCVTIMEO, 50)

if "notify_telemetry_pull_bound" in globals():
    notify_telemetry_pull_bound()


def _terminate_process():
    """Exit the whole executable (C++ model threads + Python) on GUI close."""
    os._exit(0)


class TelemetryPlotter:
    """Series keyed by (ied, graph_name, line_name); 2-column layout by IED."""

    def __init__(self):
        self._series = {}  # key -> {"t": list, "y": list, "style": str}
        self._gname_order = []
        self._fig = plt.figure(figsize=(14, 9))
        self._fig.canvas.manager.set_window_title("IED telemetry")
        self._fig.canvas.mpl_connect("close_event", lambda _ev: _terminate_process())
        self._axes = {}
        self._artists = {}
        self._layout_key = None
        self._last_draw_t = 0.0

    def _register_graph(self, _ied, gname):
        if gname not in self._gname_order:
            self._gname_order.append(gname)

    def _layout_fingerprint(self):
        ieds = sorted({k[0] for k in self._series.keys()})
        col_ieds = ieds[:2] if len(ieds) >= 2 else ieds
        return (tuple(self._gname_order), tuple(col_ieds))

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

        self._fig.clf()
        self._fig.canvas.mpl_connect("close_event", lambda _ev: _terminate_process())
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

        self._fig.tight_layout()
        self._recreate_all_artists()
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
        self._apply_shared_xlim()

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

    def _sync_all_artists_data(self):
        for key, data in self._series.items():
            ln = self._artists.get(key)
            if ln is None:
                continue
            if not data["t"]:
                ln.set_data([], [])
                continue
            ln.set_data(data["t"], data["y"])

    def _apply_shared_xlim(self):
        all_t = []
        for data in self._series.values():
            if data["t"]:
                all_t.extend(data["t"])
        if not all_t:
            return
        t0, t1 = min(all_t), max(all_t)
        if t1 <= t0:
            t1 = t0 + 1e-12
        pad = (t1 - t0) * 0.02 + 1e-12
        lo, hi = t0 - pad, t1 + pad
        for ax in self._axes.values():
            ax.set_xlim(lo, hi)

    def ingest_message(self, msg):
        ied = msg["name"]
        t = float(msg["t"])
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
        for ax in self._axes.values():
            ax.relim(visible_only=True)
            ax.autoscale_view(scalex=False, scaley=True)
        self._apply_shared_xlim()
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

    while True:
        if plotter._fig.number not in plt.get_fignums():
            _terminate_process()

        try:
            batch = _recv_batch(sock)
        except zmq.Again:
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
