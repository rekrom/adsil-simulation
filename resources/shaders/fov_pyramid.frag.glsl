#version 330 core
        uniform vec3 color;
        out vec4 FragColor;
        uniform float alpha;
        void main() {
            FragColor = vec4(color, alpha);
        }