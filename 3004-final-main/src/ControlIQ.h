#ifndef CONTROLIQ_H
#define CONTROLIQ_H

// Blood glucose thresholds in mmol/L
namespace ControlIQ {
    constexpr float BG_VERY_LOW = 3.9f;   // 70 mg/dL
    constexpr float BG_LOW = 6.25f;       // 112.5 mg/dL
    constexpr float BG_HIGH = 8.9f;       // 160 mg/dL
    constexpr float BG_VERY_HIGH = 10.0f; // 180 mg/dL

    enum class State {
        NORMAL,             // Normal range (6.25-8.9 mmol/L)
        DECREASING,         // Decreasing insulin (≤6.25 mmol/L)
        STOPPED,            // Stopped basal insulin (<3.9 mmol/L)
        INCREASING,         // Increasing insulin (>8.9 mmol/L)
        AUTO_CORRECTION     // Automatic correction bolus (≥10.0 mmol/L)
    };

    enum class Icon {
        GRAY_DIAMOND,           // Normal range
        ORANGE_DIAMOND_BOTTOM,  // Decreasing insulin
        RED_DIAMOND_BOTTOM,     // Stopped basal
        BLUE_DIAMOND_TOP,       // Increasing insulin
        AUTO_CORRECTION_BOLUS   // White droplet in blue square
    };
}

#endif // CONTROLIQ_H
