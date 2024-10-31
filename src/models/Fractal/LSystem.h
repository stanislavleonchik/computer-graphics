#pragma once
#include <vector>
#include <string>
#include <unordered_map>
#include "FractalParams.h"
#include "TurtleState.h"

struct LSystem {
    std::string name;
    std::string axiom;
    double angle;
    std::unordered_map<char, std::string> rules;
    FractalParams default_params;
};

inline std::vector<LSystem> get_predefined_l_systems() {
    std::vector<LSystem> l_systems;

    // Koch Curve
    LSystem koch_snowflake;
    koch_snowflake.name = "Koch Snowflake";
    koch_snowflake.axiom = "F++F++F";
    koch_snowflake.angle = 60.0;
    koch_snowflake.rules['F'] = "F-F++F-F"; //
    // Default parameters
    koch_snowflake.default_params = FractalParams{
            1,      // lines_per_frame
            4,      // iterations
            4,      // initial_length
            1,      // initial_width
            1.0f,      // width_reduce
            1.0f,      // length_reduce
            {185,30,230}, // begin_color (SaddleBrown)
            {200,180,208}, // end_color (ForestGreen)
            0.0f,  // angle_variation
            60.0   // angle
    };
    l_systems.push_back(koch_snowflake);

    // Koch Curve
    LSystem koch_curve;
    koch_curve.name = "Koch Curve";
    koch_curve.axiom = "F";
    koch_curve.angle = 60.0;
    koch_curve.rules['F'] = "F-F++F-F"; //
    // Default parameters
    koch_curve.default_params = FractalParams{
            1,      // lines_per_frame
            4,      // iterations
            4,      // initial_length
            1,      // initial_width
            1.0f,      // width_reduce
            1.0f,      // length_reduce
            {185,30,230}, // begin_color (SaddleBrown)
            {200,180,208}, // end_color (ForestGreen)
            0.0f,  // angle_variation
            60.0   // angle
    };
    l_systems.push_back(koch_curve);

    // Square Koch Island
    LSystem square_koch_island;
    square_koch_island.name = "Square Koch Island";
    square_koch_island.axiom = "F+F+F+F";
    square_koch_island.angle = 90.0;
    square_koch_island.rules['F'] = "F+F-F-F+F";
    // Default parameters
    square_koch_island.default_params = FractalParams{
            1, // lines_per_frame
            4, // iterations
            4, // initial_length
            1, // initial_width
            1.0f, // width_reduce
            1.0f, // length_reduce
            {185,30,230}, // begin_color (SaddleBrown)
            {200,180,208}, // end_color (ForestGreen)
            0.0f, // angle_variation
            90.0 // angle
    };
    l_systems.push_back(square_koch_island);

    // Sierpinski Carpet
    LSystem sierpinski_carpet;
    sierpinski_carpet.name = "Sierpinski Carpet";
    sierpinski_carpet.axiom = "FXF--FF--FF";
    sierpinski_carpet.angle = 60.0;
    sierpinski_carpet.rules['F'] = "FF";
    sierpinski_carpet.rules['X'] = "--FXF++FXF--";
    // Default parameters
    sierpinski_carpet.default_params = FractalParams{
            1, // lines_per_frame
            5, // iterations
            6, // initial_length
            1, // initial_width
            1.0f, // width_reduce
            1.0f, // length_reduce
            {185,30,230}, // begin_color (SaddleBrown)
            {200,180,208}, // end_color (ForestGreen)
            0.0f, // angle_variation
            60.0 // angle
    };
    l_systems.push_back(sierpinski_carpet);

    // Hilbert Curve
    LSystem hilbert_curve;
    hilbert_curve.name = "Hilbert Curve";
    hilbert_curve.axiom = "X";
    hilbert_curve.angle = 90.0;
    hilbert_curve.rules['F'] = "F";
    hilbert_curve.rules['X'] = "-YF+XFX+FY-";
    hilbert_curve.rules['Y'] = "+XF-YFY-FX+";
    // Default parameters
    hilbert_curve.default_params = FractalParams{
            1, // lines_per_frame
            5, // iterations
            5, // initial_length
            1, // initial_width
            1.0f, // width_reduce
            1.0f, // length_reduce
            {185,30,230}, // begin_color (SaddleBrown)
            {200,180,208}, // end_color (ForestGreen)
            0.0f, // angle_variation
            90.0 // angle
    };
    l_systems.push_back(hilbert_curve);

    // Dragon Curve
    LSystem dragon_curve;
    dragon_curve.name = "Dragon Curve";
    dragon_curve.axiom = "FX";
    dragon_curve.angle = 90.0;
    dragon_curve.rules['F'] = "F";
    dragon_curve.rules['X'] = "X+YF+";
    dragon_curve.rules['Y'] = "-FX-Y";
    // Default parameters
    dragon_curve.default_params = FractalParams{
            1, // lines_per_frame
            10, // iterations
            10, // initial_length
            1, // initial_width
            1.0f, // width_reduce
            1.0f, // length_reduce
            {185,30,230}, // begin_color (SaddleBrown)
            {200,180,208}, // end_color (ForestGreen)
            0.0f, // angle_variation
            90.0 // angle
    };
    l_systems.push_back(dragon_curve);

    // Gosper's Hexagonal Curve
    LSystem gosper_hexagonal_curve;
    gosper_hexagonal_curve.name = "Gosper's Hexagonal Curve";
    gosper_hexagonal_curve.axiom = "XF";
    gosper_hexagonal_curve.angle = 60.0;
    gosper_hexagonal_curve.rules['F'] = "F";
    gosper_hexagonal_curve.rules['X'] = "X+YF++YF-FX--FXFX-YF+";
    gosper_hexagonal_curve.rules['Y'] = "-FX+YFYF++YF+FX--FX-Y";
    // Default parameters
    gosper_hexagonal_curve.default_params = FractalParams{
            1,  // lines_per_frame
            3, // iterations
            8, // initial_length
            1, // initial_width
            1.0f, // width_reduce
            1.0f, // length_reduce
            {185,30,230}, // begin_color (SaddleBrown)
            {200,180,208}, // end_color (ForestGreen)
            0.0f, // angle_variation
            60.0 // angle
    };
    l_systems.push_back(gosper_hexagonal_curve);

    // Куст 1
    LSystem bush1;
    bush1.name = "Bush 1";
    bush1.axiom = "F";
    bush1.angle = 22.0;
    bush1.rules['F'] = "FF-[−F+F+F]+[+F−F−F]";
// Default parameters
    bush1.default_params = FractalParams{
            1,      // lines_per_frame
            4,      // iterations
            6,      // initial_length
            1,      // initial_width
            1.0f,   // width_reduce
            1.0f,   // length_reduce
            {139, 69, 19}, // begin_color (SaddleBrown)
            {34, 139, 34}, // end_color (ForestGreen)
            0.0f,   // angle_variation
            22.0f   // angle
    };
    l_systems.push_back(bush1);

    // Куст 2
    LSystem bush2;
    bush2.name = "Bush 2";
    bush2.axiom = "X";
    bush2.angle = 20.0;
    bush2.rules['F'] = "FF";
    bush2.rules['X'] = "F[+X]F[−X]+X";
// Default parameters
    bush2.default_params = FractalParams{
            1,      // lines_per_frame
            5,      // iterations
            6,      // initial_length
            1,      // initial_width
            1.0f,   // width_reduce
            1.0f,   // length_reduce
            {139, 69, 19}, // begin_color (SaddleBrown)
            {34, 139, 34}, // end_color (ForestGreen)
            0.0f,   // angle_variation
            20.0f   // angle
    };
    l_systems.push_back(bush2);

    // Куст 3
    LSystem bush3;
    bush3.name = "Bush 3";
    bush3.axiom = "X";
    bush3.angle = 22.5;
    bush3.rules['F'] = "FF";
    bush3.rules['X'] = "F-[[X]+X]+F[+FX]-X";
// Default parameters
    bush3.default_params = FractalParams{
            1,      // lines_per_frame
            5,      // iterations
            6,      // initial_length
            1,      // initial_width
            1.0f,   // width_reduce
            1.0f,   // length_reduce
            {139, 69, 19}, // begin_color (SaddleBrown)
            {34, 139, 34}, // end_color (ForestGreen)
            0.0f,   // angle_variation
            22.5f   // angle
    };
    l_systems.push_back(bush3);

    // Шестиугольная мозаика
    LSystem hex_mosaic;
    hex_mosaic.name = "Hexagonal Mosaic";
    hex_mosaic.axiom = "X";
    hex_mosaic.angle = 60.0;
    hex_mosaic.rules['F'] = "F";
    hex_mosaic.rules['X'] = "[−F+F[Y]+F][+F−F[X]−F]";
    hex_mosaic.rules['Y'] = "[−F+F[Y]+F][+F−F−F]";
// Default parameters
    hex_mosaic.default_params = FractalParams{
            1,      // lines_per_frame
            6,      // iterations
            30,      // initial_length
            1,      // initial_width
            1.0f,   // width_reduce
            1.0f,   // length_reduce
            {139, 69, 19}, // begin_color (SaddleBrown)
            {34, 139, 34}, // end_color (ForestGreen)
            0.0f,   // angle_variation
            60.0f   // angle
    };
    l_systems.push_back(hex_mosaic);

    // Определение дерева
    LSystem tree;
    tree.name = "Random Tree";
    tree.axiom = "X";
    tree.angle = 25.0;
    tree.rules['X'] = "F[@[-X]+X]";
    tree.rules['F'] = "FF";
// Параметры фрактала для дерева
    tree.default_params = FractalParams{
            1,              // lines_per_frame
            8,              // iterations
            0.5,              // initial_length (начальная длина)
            1,              // initial_width (начальная ширина)
            1.0f,           // width_reduce (коэффициент уменьшения ширины)
            1.0f,          // length_reduce (коэффициент уменьшения длины)
            {85, 107, 47},  // begin_color (темно-зеленый, цвет начала ствола)
            {34, 139, 34},  // end_color (светло-зеленый, цвет кончика ветвей)
            0.0f,          // angle_variation (дополнительная вариация угла)
            -20.0f           // angle (основной угол для поворота ветвей)
    };
    l_systems.push_back(tree);

    return l_systems;
}
