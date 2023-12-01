//
// Created by Tony Adriansen on 11/29/23.
//

#include "CrossHair.h"
#include "../Config.h"

CrossHair::CrossHair() : VAO(0), VBO(0) {
    std::cout << "Creating crosshair" << std::endl;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    std::array<float, 8> vertices = {
            -0.75f,  0.0f,
            0.75f,  0.0f,
            0.0f,   1.0f,
            0.0f,  -1.0f
    };

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), nullptr);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

}
