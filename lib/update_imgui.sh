#!/bin/bash

mkdir tmp_newImgui
git clone https://github.com/ocornut/imgui.git tmp_newImgui

find tmp_newImgui/ -name \*.cpp -exec sed -i "s;#include \"img;#include \"mh/ext/imgui/img;g" {} \;
find tmp_newImgui/ -name \*.h -exec sed -i "s;#include \"img;#include \"mh/ext/imgui/img;g" {} \;

find tmp_newImgui/ -name \*.cpp -exec sed -i "s;#include \"stb;#include \"mh/ext/imgui/stb;g" {} \;
find tmp_newImgui/ -name \*.h -exec sed -i "s;#include \"stb;#include \"mh/ext/imgui/stb;g" {} \;
