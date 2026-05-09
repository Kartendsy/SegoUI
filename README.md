# SegoUI
SegoUI is a header-only user interface (UI) library for SDL3 and GLM. Its architecture uses a scene-tree system inspired by the node-based logic of the Godot Engine.


## Key Features

Header-Only: Easy integration by including just one header file.

Scene-Tree Architecture: Parent-Child system with automatic (recursive) global position calculation.

Auto-ID: Uses the FNV-1a string hashing algorithm for node identification without manual input.

GLM Integration: Leverages the glm::vec2 and glm::vec4 data types for precise position and color calculations.

Container System: VBoxContainer and HBoxContainer are available for automatic layout.

## System Requirements

SDL3

SDL3_ttf

GLM

## Installation

Copy the SegoUI.hpp file into your project directory and ensure the SDL3 and GLM libraries are linked in your build system.
