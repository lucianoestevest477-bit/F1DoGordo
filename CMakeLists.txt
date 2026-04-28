cmake_minimum_required(VERSION 3.22)

project(F1DoGordo VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(FetchContent)

FetchContent_Declare(
    JUCE
    GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
    GIT_TAG 8.0.10
)

FetchContent_MakeAvailable(JUCE)

juce_add_plugin(F1DoGordo
    COMPANY_NAME "Gordo Audio"
    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT FALSE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS FALSE
    COPY_PLUGIN_AFTER_BUILD FALSE
    PLUGIN_MANUFACTURER_CODE GdAu
    PLUGIN_CODE FDoG
    FORMATS VST3
    PRODUCT_NAME "F1 do Gordo"
)

target_sources(F1DoGordo
    PRIVATE
        Source/PluginProcessor.cpp
        Source/PluginEditor.cpp
        Source/Parameters.cpp
        Source/DSP/ChannelModule.cpp
        Source/DSP/FETCompressorModule.cpp
        Source/DSP/AirExciterModule.cpp
        Source/DSP/DelayModule.cpp
        Source/DSP/ReverbModule.cpp
        Source/DSP/Metering.cpp
        Source/UI/F1Theme.cpp
        Source/UI/GordoKnob.cpp
        Source/UI/GordoButton.cpp
        Source/UI/GordoLED.cpp
        Source/UI/GordoMeter.cpp
        Source/UI/F1TopBar.cpp
        Source/UI/F1Dashboard.cpp
)

target_compile_definitions(F1DoGordo
    PUBLIC
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
        JUCE_VST3_CAN_REPLACE_VST2=0
)

target_link_libraries(F1DoGordo
    PRIVATE
        juce::juce_dsp
        juce::juce_audio_utils
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags
)
