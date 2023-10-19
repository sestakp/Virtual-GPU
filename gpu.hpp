/*!
 * @file
 * @brief This file contains class that represents graphic card.
 *
 * @author Tomáš Milet, imilet@fit.vutbr.cz
 */
#pragma once

#include <student/fwd.hpp>
#include <student/gpu.hpp>
#include <vector>


/**
 * @brief This class represent software GPU
 */
class GPU{
  public:
    GPU();
    virtual ~GPU();


    /**
      * @brief This struct represent Buffer on software GPU
    */
    struct Buffer_s{
      void *data;
      uint64_t length;  
    };

    #define FRAC_BITS 8

    /**
      * @brief This struct represent Reading head for VertexPuller on software GPU
    */
    struct VertexPullerHead_s{
      BufferID bufferID;
      uint64_t offset;
      uint64_t stride;
      AttributeType attributeType;
      bool enabled = false;
    };

    /**
      * @brief This struct represent VertexPuller on software GPU
    */
    struct VertexPuller_s{
      BufferID bufferID = -1;
      IndexType indexType;
      VertexPullerHead_s VP_Heads[maxAttributes];
    };
    
    /**
      * @brief This struct represent Program_s on software GPU
    */
    struct Program_s{
      VertexShader vertexShader;
      FragmentShader fragmentShader;
      Uniforms uniforms;
      AttributeType attributeType[maxAttributes];
      bool exist = false;
      };


    struct FrameBuffer_s{
      uint8_t *colorBuffer;
      float *depthBuffer;
      uint32_t width;
      uint32_t height;
    };

    struct Triangle_s{
      OutVertex vertex[3];
    };

    struct RasterizationBox_s{
        uint32_t min_x;
        uint32_t min_y;
        uint32_t max_x;
        uint32_t max_y;
        uint32_t width;
        uint32_t height;
    };


    //buffer object commands
    BufferID  createBuffer           (uint64_t size);
    void      deleteBuffer           (BufferID buffer);
    void      setBufferData          (BufferID buffer,uint64_t offset,uint64_t size,void const* data);
    void      getBufferData          (BufferID buffer,uint64_t offset,uint64_t size,void      * data);
    bool      isBuffer               (BufferID buffer);

    //vertex array object commands (vertex puller)
    ObjectID  createVertexPuller     ();
    void      deleteVertexPuller     (VertexPullerID vao);
    void      setVertexPullerHead    (VertexPullerID vao,uint32_t head,AttributeType type,uint64_t stride,uint64_t offset,BufferID buffer);
    void      setVertexPullerIndexing(VertexPullerID vao,IndexType type,BufferID buffer);
    void      enableVertexPullerHead (VertexPullerID vao,uint32_t head);
    void      disableVertexPullerHead(VertexPullerID vao,uint32_t head);
    void      bindVertexPuller       (VertexPullerID vao);
    void      unbindVertexPuller     ();
    bool      isVertexPuller         (VertexPullerID vao);

    //program object commands
    ProgramID createProgram          ();
    void      deleteProgram          (ProgramID prg);
    void      attachShaders          (ProgramID prg,VertexShader vs,FragmentShader fs);
    void      setVS2FSType           (ProgramID prg,uint32_t attrib,AttributeType type);
    void      useProgram             (ProgramID prg);
    bool      isProgram              (ProgramID prg);
    void      programUniform1f       (ProgramID prg,uint32_t uniformId,float     const&d);
    void      programUniform2f       (ProgramID prg,uint32_t uniformId,glm::vec2 const&d);
    void      programUniform3f       (ProgramID prg,uint32_t uniformId,glm::vec3 const&d);
    void      programUniform4f       (ProgramID prg,uint32_t uniformId,glm::vec4 const&d);
    void      programUniformMatrix4f (ProgramID prg,uint32_t uniformId,glm::mat4 const&d);

    //framebuffer functions
    void      createFramebuffer      (uint32_t width,uint32_t height);
    void      deleteFramebuffer      ();
    void      resizeFramebuffer      (uint32_t width,uint32_t height);
    uint8_t*  getFramebufferColor    ();
    float*    getFramebufferDepth    ();
    uint32_t  getFramebufferWidth    ();
    uint32_t  getFramebufferHeight   ();

    //execution commands
    void      clear                  (float r,float g,float b,float a);
    void      drawTriangles          (uint32_t  nofVertices);

    /// \addtogroup gpu_init 00. proměnné, inicializace / deinicializace grafické karty
    /// @{
    /// \todo zde si můžete vytvořit proměnné grafické karty (buffery, programy, ...)
    /// @}


  private:
      
      void      _VertexPuller          (InVertex &inVertex, uint32_t iterator);
      void      _VertexProcessor       (OutVertex &outVertex,InVertex &inVertex, uint32_t iterator);
      void      _PrimitiveAssembly     (std::vector<Triangle_s> &Triangles,uint32_t  nofTriangles);
      int       _Clipping              (Triangle_s &triangle);
      void      _Clipping1V            (Triangle_s &triangle, bool *ClipVertex);
      void      _Clipping2V            (Triangle_s &triangle, bool *ClipVertex);
      void      _PerspectiveDivision   (Triangle_s &triangle);
      void      _ViewPortTransform     (Triangle_s &triangle);

      float     _FindMin               (float a, float b, float c);
      float     _FindMax               (float a, float b, float c);
      void      _Rasterize             (Triangle_s &triangle);

      void      _FragmentProcessor     (InFragment &inFragment);

      //Array of buffers, BufferID == index
      std::vector<GPU::Buffer_s> BufferList;

      //Array of vertex pullers
      std::vector<GPU::VertexPuller_s> VertexPullerList;

      uint64_t ActiveVertexPullerID = emptyID;


      std::vector<InVertex> VertexBuffer;
      uint32_t VertexBufferID = emptyID;

      //Array of programs
      std::vector<GPU::Program_s> ProgramList;
      uint64_t ActiveProgramID = emptyID;


      GPU::FrameBuffer_s FrameBuffer;
};


