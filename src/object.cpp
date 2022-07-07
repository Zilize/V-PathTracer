#include "object.h"

#include <utility>
#include "obj_loader.h"

Object::Object(const string &filename, std::shared_ptr<Material> _material) {
    objl::Loader loader;
    if (!loader.LoadFile(filename)) {
        throw std::runtime_error("Load file error!");
    }
    if (loader.LoadedMeshes.size() != 1) {
        throw std::runtime_error("More than one object in an obj file.");
    }
    auto mesh = loader.LoadedMeshes[0];

    for (int i = 0; i < mesh.Vertices.size(); i += 3) {
        Triangle currentTriangle(
            vec3(mesh.Vertices[i + 0].Position.X, mesh.Vertices[i + 0].Position.Y, mesh.Vertices[i + 0].Position.Z),
            vec3(mesh.Vertices[i + 1].Position.X, mesh.Vertices[i + 1].Position.Y, mesh.Vertices[i + 1].Position.Z),
            vec3(mesh.Vertices[i + 2].Position.X, mesh.Vertices[i + 2].Position.Y, mesh.Vertices[i + 2].Position.Z),
            vec3(mesh.Vertices[i + 0].Normal.X, mesh.Vertices[i + 0].Normal.Y, mesh.Vertices[i + 0].Normal.Z),
            vec3(mesh.Vertices[i + 1].Normal.X, mesh.Vertices[i + 1].Normal.Y, mesh.Vertices[i + 1].Normal.Z),
            vec3(mesh.Vertices[i + 2].Normal.X, mesh.Vertices[i + 2].Normal.Y, mesh.Vertices[i + 2].Normal.Z)
        );
        primitives.emplace_back(currentTriangle);
    }

    material = std::move(_material);
}