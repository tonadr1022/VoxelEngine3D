//
// Created by Tony Adriansen on 11/15/23.
//

#ifndef VOXEL_ENGINE_APPLICATION_H
#define VOXEL_ENGINE_APPLICATION_H


class Application {
public:
    Application();

    void run();
private:
    bool m_running{};
    void init_opengl();
    void init_glfw();

};


#endif //VOXEL_ENGINE_APPLICATION_H
