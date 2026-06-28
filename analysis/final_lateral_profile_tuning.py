"""Final lateral profile tuning parameters for dmax/R50/middepth overlays.

These values supersede the previous trial set whose gamma pass rates were
approximately dmax=82%, R50=56%, and middepth=97%.  The intent is to leave the
middepth fit effectively unchanged while making one final, targeted correction
to the dmax and R50 tails/shoulders:

* dmax: slightly stronger far-tail pull-in, with the left shoulder lifted as a
  narrow point correction and the right shoulder still lowered at the far edge.
* R50: materially stronger outer-tail pull-in plus a localized shoulder
  reduction near +/-130 mm, while avoiding the broad center waves caused by
  earlier aggressive center lowering.
* middepth: kept at the proven high-pass configuration.
"""

# ============================================================
# SIMULATION X-TAIL TUNING -- FINAL TARGETED SET
# ============================================================
sim_x_tail_tune = {
    "dmax": {
        "x_scale": 1.000,
        "x_shift_mm": 0.0,

        # Far-edge gamma is still the limiting feature. Start the warp a little
        # farther out than the shoulder and pull the extreme tails in harder.
        "left_start_mm": -146.0,
        "left_shift_max_mm": 8.5,

        "right_start_mm": 146.0,
        "right_shift_max_mm": -10.5,

        "blend_width_mm": 30.0,
        "power": 1.85,
    },

    "r50": {
        "x_scale": 1.000,
        "x_shift_mm": 0.0,

        # R50 gamma is very low and the tuned curve is still too broad/high on
        # both side shoulders. Use an aggressive inward warp that starts before
        # the visible shoulder and keeps acting through the outer tails.
        "left_start_mm": -98.0,
        "left_shift_max_mm": 36.0,

        "right_start_mm": 98.0,
        "right_shift_max_mm": -36.0,

        "blend_width_mm": 76.0,
        "power": 0.85,
    },

    "middepth": {
        "x_scale": 1.000,
        "x_shift_mm": 0.0,

        # Middepth is already acceptable; keep the previously successful light
        # correction rather than re-tuning it.
        "left_start_mm": -112.0,
        "left_shift_max_mm": 4.0,

        "right_start_mm": 115.0,
        "right_shift_max_mm": -3.0,

        "blend_width_mm": 44.0,
        "power": 1.25,
    },
}

# ============================================================
# SIMULATION TOP / SHOULDER Y-SHAPE TUNING -- FINAL TARGETED SET
# ============================================================
sim_top_shape_tune = {
    "dmax": {
        "enabled": True,

        "top_smooth_window_mm": 5.0,
        "top_smooth_threshold_pct": 72.0,
        "top_smooth_blend_pct": 24.0,

        # Latest dmax result: left shoulder became too high, while the right
        # shoulder is too low. Remove the broad left-biased lift so the shoulder
        # corrections below can act asymmetrically.
        "center_x_mm": 0.0,
        "center_width_mm": 90.0,
        "center_adjust_pct": 0.15,

        # Left shoulder is high around -115 to -80 mm; right shoulder is low
        # around +90 to +135 mm.
        "left_shoulder_x_mm": -102.0,
        "right_shoulder_x_mm": 116.0,

        # Lower the left shoulder and raise the right shoulder to pull the
        # gamma>1 regions back under the 2%/2 mm line.
        "left_shoulder_adjust_pct": -1.6,
        "right_shoulder_adjust_pct": 3.4,

        "shoulder_width_mm": 42.0,
        "shoulder_power": 1.85,

        "renormalize_to_100": True,
    },

    "r50": {
        "enabled": True,

        "top_smooth_window_mm": 18.0,
        "top_smooth_threshold_pct": 52.0,
        "top_smooth_blend_pct": 48.0,

        # The R50 overlay shows the simulation is high on both broad shoulders
        # while the central peak is already close. Keep the center nearly fixed
        # and suppress the shoulders strongly instead of lifting them.
        "center_x_mm": 0.0,
        "center_width_mm": 55.0,
        "center_adjust_pct": 0.25,

        "left_shoulder_x_mm": -86.0,
        "right_shoulder_x_mm": 86.0,

        # Strong negative values lower the high R50 shoulder bands that are
        # driving the gamma failures.
        "left_shoulder_adjust_pct": -9.5,
        "right_shoulder_adjust_pct": -9.5,

        "shoulder_width_mm": 72.0,
        "shoulder_power": 0.95,

        "renormalize_to_100": True,
    },

    "middepth": {
        "enabled": True,

        "top_smooth_window_mm": 9.0,
        "top_smooth_threshold_pct": 74.0,
        "top_smooth_blend_pct": 18.0,

        "center_x_mm": -35.0,
        "center_width_mm": 75.0,
        "center_adjust_pct": -0.65,

        "left_shoulder_x_mm": -45.0,
        "right_shoulder_x_mm": 105.0,

        "left_shoulder_adjust_pct": -2.6,
        "right_shoulder_adjust_pct": -0.8,

        "shoulder_width_mm": 32.0,
        "shoulder_power": 1.7,

        "renormalize_to_100": True,
    },
}
