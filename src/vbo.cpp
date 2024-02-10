#include "vbo.hpp"  

#include "file.hpp"  
#include "entry.hpp"  

#include <GL/gl3w.h>
#include <GLFW/glfw3.h>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

VBO::VBO() {  

    indices = buffer.addObj(indices_s);

    vertices = buffer.addObj(vertices_s);

    create();
}

void VBO::destroy() {

    if (vbo<0) return;

    glDisableVertexAttribArray(0); 
    glDisableVertexAttribArray(1); 
    glDisableVertexAttribArray(2); 
    glDisableVertexAttribArray(3); 

    glDeleteBuffers(1, &vbo);
    glDeleteBuffers(1, &ibo);
    glDeleteVertexArrays(1, &vao);

}

void VBO::create() {

    destroy();

    glGenBuffers(1, &vbo); glGenBuffers(1, &ibo); glGenVertexArrays(1, &vao);

}

VBO::~VBO()  { destroy(); }

void VBO::upload() {

    glBindVertexArray(vao);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER,  vertices->size(), vertices->data(), GL_STATIC_DRAW );

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices->size(), indices->data(), GL_STATIC_DRAW );

    // make this parametric from Object vertices definition
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_TRUE, vertices->s->size, (GLvoid *) 0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_TRUE, vertices->s->size, (GLvoid *) (2*sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(3, 1, GL_FLOAT, GL_TRUE, vertices->s->size, (GLvoid *) (4*sizeof(float)));
    glEnableVertexAttribArray(3);

}

void VBO::draw(int count) {

    glBindVertexArray(vao); 

    glDrawElementsInstanced(GL_TRIANGLES, indices->size(), GL_UNSIGNED_INT, 0, count);

}
	

void VBO::import(File *file) {    

    Assimp::Importer importer;

    const aiScene* scene = importer.ReadFileFromMemory(&file->data[0], file->data.size(), aiProcess_CalcTangentSpace       | 
        aiProcess_Triangulate            |
        aiProcess_JoinIdenticalVertices  |
        aiProcess_SortByPType);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) PLOGW << "Failed to load OBJ file: " << importer.GetErrorString();

    auto mesh = scene->mMeshes[0];

    for (int i = 0; i < mesh->mNumVertices; i++) {

        const aiVector3D& vertex = mesh->mVertices[i];
        
        auto v = vertices->push();

        // v["Position"]["x"].set<float>(vertex.x);
        // v["Position"]["y"].set<float>(vertex.y);

        // v["UV"]["x"].set<float>(mesh->mTextureCoords[0][i].x);
        // v["UV"]["y"].set<float>(mesh->mTextureCoords[0][i].y);

    }
    
    for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
        
        const aiFace& face = mesh->mFaces[i];

        auto indices = this->indices->push();


        PLOGD << (uint32_t)face.mIndices[0];
        PLOGD << (uint32_t)face.mIndices[1];
        PLOGD << (uint32_t)face.mIndices[2];


        // indices[0][0].set<uint32_t>(face.mIndices[0]);
        // indices[0][1].set<uint32_t>(face.mIndices[1]);
        // indices[0][2].set<uint32_t>(face.mIndices[2]);

    }
    
    upload();
}