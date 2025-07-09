  #version 330 core
        in vec3 vColor;
        out vec4 FragColor;
        uniform bool useUniformColor;
        uniform vec3 uniformColor;
        uniform float alpha;
        void main() 
        {
            vec3 finalColor = useUniformColor ? uniformColor : vColor;
            FragColor = vec4(finalColor, alpha);
        }