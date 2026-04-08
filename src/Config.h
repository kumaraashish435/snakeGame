#pragma once

namespace Config {
    // Grid dimensions (cells)
    constexpr int GridWidth  = 20;
    constexpr int GridHeight = 20;

    // Default window size (pixels)
    constexpr int InitialWindowWidth  = 720;
    constexpr int InitialWindowHeight = 760; 

    // Gameplay
    constexpr float InitialSpeed   = 8.0f;   
    constexpr float MaxSpeed        = 20.0f;
    constexpr float SpeedIncrement  = 0.4f;   

    // Rendering
    constexpr int   TargetFPS       = 60;
    constexpr float ScoreBarHeight  = 44.0f;  
    constexpr float CellPadding     = 0.06f;  
}
