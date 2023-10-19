/*!
 * @file
 * @brief This file contains implementation of gpu
 *
 * @author Tomáš Milet, imilet@fit.vutbr.cz
 */

#include <student/gpu.hpp>
#include <vector>
#include <algorithm> //find,
#include <cstring> //memcpy,
#include <iostream> //error messages

/// \addtogroup gpu_init
/// @{

/**
 * @brief Constructor of GPU
 */
GPU::GPU(){
  /// \todo Zde můžete alokovat/inicializovat potřebné proměnné grafické karte

}

/**
 * @brief Destructor of GPU
 */
GPU::~GPU(){
  /// \todo Zde můžete dealokovat/deinicializovat grafickou kartu

}

/// @}

/** \addtogroup buffer_tasks 01. Implementace obslužných funkcí pro buffery
 * @{
 */


/**
 * @brief This function allocates buffer on GPU.
 *
 * @param size size in bytes of new buffer on GPU.
 *
 * @return unique identificator of the buffer
 */
BufferID GPU::createBuffer(uint64_t size) { 
  /// \todo Tato funkce by měla na grafické kartě vytvořit buffer dat.<br>
  /// Velikost bufferu je v parameteru size (v bajtech).<br>
  /// Funkce by měla vrátit unikátní identifikátor identifikátor bufferu.<br>
  /// Na grafické kartě by mělo být možné alkovat libovolné množství bufferů o libovolné velikosti.<br>

    //Get ID for next buffer
    //return first NULL value in BufferList  
    auto it = find_if(BufferList.begin(), BufferList.end(), [](auto x) { return x.data == NULL; });
    BufferID Buffer_id = distance(BufferList.begin(), it);
    
    //Check if some null value was found, if not resize vector
    if(Buffer_id == BufferList.size())
      try
      {
        BufferList.resize(BufferList.size()+1);
      }
      catch(std::bad_alloc& e)
      {
        std::cerr << "Function createBuffer: bad_alloc problem with resize" << e.what() << '\n';
        return emptyID;
      }
      
    //Fill buffer
    BufferList[Buffer_id].data = malloc(size);
    BufferList[Buffer_id].length = size;            

    //return emptyID if malloc wasn't sucesfull  
    if(BufferList[Buffer_id].data != NULL)
      return Buffer_id;

    return emptyID;
}


/**
 * @brief This function frees allocated buffer on GPU.
 *
 * @param buffer buffer identificator
 */
void GPU::deleteBuffer(BufferID buffer) 
{
  /// \todo Tato funkce uvolní buffer na grafické kartě.
  /// Buffer pro smazání je vybrán identifikátorem v parameteru "buffer".
  /// Po uvolnění bufferu je identifikátor volný a může být znovu použit při vytvoření nového bufferu.
  
  if(!isBuffer(buffer))
    return;

  free(BufferList[buffer].data);
  BufferList[buffer].data = NULL;
  
  //If we deleted last item in vector, we can resize it
  if(buffer == BufferList.size())
    BufferList.resize(BufferList.size()-1);
  
}


/**
 * @brief This function uploads data to selected buffer on the GPU
 *
 * @param buffer buffer identificator
 * @param offset specifies the offset into the buffer's data
 * @param size specifies the size of buffer that will be uploaded
 * @param data specifies a pointer to new data
 */
void GPU::setBufferData(BufferID buffer, uint64_t offset, uint64_t size, void const* data)
{
  /// \todo Tato funkce nakopíruje data z cpu na "gpu".<br>
  /// Data by měla být nakopírována do bufferu vybraného parametrem "buffer".<br>
  /// Parametr size určuje, kolik dat (v bajtech) se překopíruje.<br>
  /// Parametr offset určuje místo v bufferu (posun v bajtech) kam se data nakopírují.<br>
  /// Parametr data obsahuje ukazatel na data na cpu pro kopírování.<br>
  
  //Check if buffer exist
  if(isBuffer(buffer) == false || BufferList[buffer].length < size+offset)
    return;

  //set pointer data to  pointer to data from Bufferlist and add offset, copy size*byte 
  std::memcpy((uint8_t*)BufferList[buffer].data+offset,(uint8_t*)data,size);
 }


 /**
 * @brief This function downloads data from GPU.
 *
 * @param buffer specfies buffer
 * @param offset specifies the offset into the buffer from which data will be returned, measured in bytes. 
 * @param size specifies data size that will be copied
 * @param data specifies a pointer to the location where buffer data is returned. 
 */
void GPU::getBufferData(BufferID buffer, uint64_t offset, uint64_t size, void* data)
{
  /// \todo Tato funkce vykopíruje data z "gpu" na cpu.
  /// Data by měla být vykopírována z bufferu vybraného parametrem "buffer".<br>
  /// Parametr size určuje kolik dat (v bajtech) se překopíruje.<br>
  /// Parametr offset určuje místo v bufferu (posun v bajtech) odkud se začne kopírovat.<br>
  /// Parametr data obsahuje ukazatel, kam se data nakopírují.<br>

  //Check if buffer exist
  if(!isBuffer(buffer))
    return;
  
  //Check if size of data is lesser or greater with buffer size
  if(BufferList[buffer].length < size+offset)
    return;
  
  //get pointer to data from Bufferlist and add offset, copy size*byte to data
  std::memcpy((uint8_t*)data,(uint8_t*)BufferList[buffer].data+offset,size);
}

/**
 * @brief This function tests if buffer exists
 *
 * @param buffer selected buffer id
 *
 * @return true if buffer points to existing buffer on the GPU.
 */
bool GPU::isBuffer(BufferID buffer) { 
  /// \todo Tato funkce by měla vrátit true pokud buffer je identifikátor existující bufferu.<br>
  /// Tato funkce by měla vrátit false, pokud buffer není identifikátor existujícího bufferu. (nebo bufferu, který byl smazán).<br>
  /// Pro emptyId vrací false.<br>
  
  return (buffer < BufferList.size() && BufferList[buffer].data != NULL);
}

/// @}

/**
 * \addtogroup vertexpuller_tasks 02. Implementace obslužných funkcí pro vertex puller
 * @{
 */

/**
 * @brief This function creates new vertex puller settings on the GPU,
 *
 * @return unique vertex puller identificator
 */
ObjectID GPU::createVertexPuller     (){
  /// \todo Tato funkce vytvoří novou práznou tabulku s nastavením pro vertex puller.<br>
  /// Funkce by měla vrátit identifikátor nové tabulky.
  /// Prázdná tabulka s nastavením neobsahuje indexování a všechny čtecí hlavy jsou vypnuté.

    //Get ID for next VertexPuller
    //return first BufferID==-1 item in VertexPullerList  
    auto it = find_if(VertexPullerList.begin(), VertexPullerList.end(), [](auto x) { return x.bufferID == -1; });
    VertexPullerID Vertex_id = distance(VertexPullerList.begin(), it);
    
    //Check if some null value was found, if not resize vector
    if(Vertex_id == VertexPullerList.size())
      try
      {
        VertexPullerList.resize(VertexPullerList.size()+1);
      }
      catch(std::bad_alloc& e)
      {
        std::cerr << "Function createVertexPuller: bad_alloc problem with resize" << e.what() << '\n';
        return emptyID;
      }
    VertexPullerList[Vertex_id].bufferID = -2;
    
    return Vertex_id;
}

/**
 * @brief This function deletes vertex puller settings
 *
 * @param vao vertex puller identificator
 */
void     GPU::deleteVertexPuller     (VertexPullerID vao){
  /// \todo Tato funkce by měla odstranit tabulku s nastavení pro vertex puller.<br>
  /// Parameter "vao" obsahuje identifikátor tabulky s nastavením.<br>
  /// Po uvolnění nastavení je identifiktátor volný a může být znovu použit.<br>
  if(!isVertexPuller(vao))
  return;

  if(ActiveVertexPullerID == vao)
    ActiveVertexPullerID = emptyID;
  
  VertexPuller_s temp;
  VertexPullerList[vao] = temp;

  if(vao == VertexPullerList.size())
    VertexPullerList.resize(VertexPullerList.size()-1);
}

/**
 * @brief This function sets one vertex puller reading head.
 *
 * @param vao identificator of vertex puller
 * @param head id of vertex puller head
 * @param type type of attribute
 * @param stride stride in bytes
 * @param offset offset in bytes
 * @param buffer id of buffer
 */
void     GPU::setVertexPullerHead    (VertexPullerID vao,uint32_t head,AttributeType type,uint64_t stride,uint64_t offset,BufferID buffer){
  /// \todo Tato funkce nastaví jednu čtecí hlavu vertex pulleru.<br>
  /// Parametr "vao" vybírá tabulku s nastavením.<br>
  /// Parametr "head" vybírá čtecí hlavu vybraného vertex pulleru.<br>
  /// Parametr "type" nastaví typ atributu, který čtecí hlava čte. Tímto se vybere kolik dat v bajtech se přečte.<br>
  /// Parametr "stride" nastaví krok čtecí hlavy.<br>
  /// Parametr "offset" nastaví počáteční pozici čtecí hlavy.<br>
  /// Parametr "buffer" vybere buffer, ze kterého bude čtecí hlava číst.<br>
  if(!isVertexPuller(vao))
  return;

  VertexPullerList[vao].VP_Heads[head].attributeType = type;
  VertexPullerList[vao].VP_Heads[head].stride = stride;
  VertexPullerList[vao].VP_Heads[head].offset = offset;
  VertexPullerList[vao].VP_Heads[head].bufferID = buffer;
}

/**
 * @brief This function sets vertex puller indexing.
 *
 * @param vao vertex puller id
 * @param type type of index
 * @param buffer buffer with indices
 */
void     GPU::setVertexPullerIndexing(VertexPullerID vao,IndexType type,BufferID buffer){
  /// \todo Tato funkce nastaví indexování vertex pulleru.
  /// Parametr "vao" vybírá tabulku s nastavením.<br>
  /// Parametr "type" volí typ indexu, který je uložený v bufferu.<br>
  /// Parametr "buffer" volí buffer, ve kterém jsou uloženy indexy.<br>
  if(!isVertexPuller(vao))
  return;
  VertexPullerList[vao].indexType = type;
  VertexPullerList[vao].bufferID = buffer;
}

/**
 * @brief This function enables vertex puller's head.
 *
 * @param vao vertex puller 
 * @param head head id
 */
void     GPU::enableVertexPullerHead (VertexPullerID vao,uint32_t head){
  /// \todo Tato funkce povolí čtecí hlavu daného vertex pulleru.<br>
  /// Pokud je čtecí hlava povolena, hodnoty z bufferu se budou kopírovat do atributu vrcholů vertex shaderu.<br>
  /// Parametr "vao" volí tabulku s nastavením vertex pulleru (vybírá vertex puller).<br>
  /// Parametr "head" volí čtecí hlavu.<br>
  if(isVertexPuller(vao))
  VertexPullerList[vao].VP_Heads[head].enabled = true;
}

/**
 * @brief This function disables vertex puller's head
 *
 * @param vao vertex puller id
 * @param head head id
 */
void     GPU::disableVertexPullerHead(VertexPullerID vao,uint32_t head){
  /// \todo Tato funkce zakáže čtecí hlavu daného vertex pulleru.<br>
  /// Pokud je čtecí hlava zakázána, hodnoty z bufferu se nebudou kopírovat do atributu vrcholu.<br>
  /// Parametry "vao" a "head" vybírají vertex puller a čtecí hlavu.<br>
  if(isVertexPuller(vao))
    VertexPullerList[vao].VP_Heads[head].enabled = false;
}

/**
 * @brief This function selects active vertex puller.
 *
 * @param vao id of vertex puller
 */
void     GPU::bindVertexPuller       (VertexPullerID vao){
  /// \todo Tato funkce aktivuje nastavení vertex pulleru.<br>
  /// Pokud je daný vertex puller aktivován, atributy z bufferů jsou vybírány na základě jeho nastavení.<br>
  if(isVertexPuller(vao))
    ActiveVertexPullerID = vao;
}

/**
 * @brief This function deactivates vertex puller.
 */
void     GPU::unbindVertexPuller     (){
  /// \todo Tato funkce deaktivuje vertex puller.
  /// To většinou znamená, že se vybere neexistující "emptyID" vertex puller.
  ActiveVertexPullerID = emptyID;
}

/**
 * @brief This function tests if vertex puller exists.
 *
 * @param vao vertex puller
 *
 * @return true, if vertex puller "vao" exists
 */
bool     GPU::isVertexPuller         (VertexPullerID vao){
  /// \todo Tato funkce otestuje, zda daný vertex puller existuje.
  /// Pokud ano, funkce vrací true.
  return ( vao < VertexPullerList.size() && VertexPullerList[vao].bufferID != -1);
}

/// @}

/** \addtogroup program_tasks 03. Implementace obslužných funkcí pro shader programy
 * @{
 */

/**
 * @brief This function creates new shader program.
 *
 * @return shader program id
 */
ProgramID        GPU::createProgram         (){
  /// \todo Tato funkce by měla vytvořit nový shader program.<br>
  /// Funkce vrací unikátní identifikátor nového proramu.<br>
  /// Program je seznam nastavení, které obsahuje: ukazatel na vertex a fragment shader.<br>
  /// Dále obsahuje uniformní proměnné a typ výstupních vertex attributů z vertex shaderu, které jsou použity pro interpolaci do fragment atributů.<br>

  auto it = find_if(ProgramList.begin(), ProgramList.end(), [](auto x) { return x.exist == false; });
    ProgramID Program_id = distance(ProgramList.begin(), it);
    
    //Check if some null value was found, if not resize vector
    if(Program_id == ProgramList.size())
      try
      {
        ProgramList.resize(ProgramList.size()+1);
      }
      catch(std::bad_alloc& e)
      {
        std::cerr << "Function createProgram: bad_alloc problem with resize" << e.what() << '\n';
        return emptyID;
      }
    ProgramList[Program_id].exist = true;
    
    return Program_id;
}

/**
 * @brief This function deletes shader program
 *
 * @param prg shader program id
 */
void             GPU::deleteProgram         (ProgramID prg){
  /// \todo Tato funkce by měla smazat vybraný shader program.<br>
  /// Funkce smaže nastavení shader programu.<br>
  /// Identifikátor programu se stane volným a může být znovu využit.<br>

  if(!isProgram(prg))
    return;

  if(ActiveProgramID == prg)
    ActiveProgramID = emptyID;

  ProgramList[prg].exist = false;
  
  //If we deleted last item in vector, we can resize it
  if(prg == ProgramList.size())
  {
    ProgramList.resize(ProgramList.size()-1);
  }
}

/**
 * @brief This function attaches vertex and frament shader to shader program.
 *
 * @param prg shader program
 * @param vs vertex shader 
 * @param fs fragment shader
 */
void             GPU::attachShaders         (ProgramID prg,VertexShader vs,FragmentShader fs){
  /// \todo Tato funkce by měla připojít k vybranému shader programu vertex a fragment shader.
  if(!isProgram(prg))
    return;
  ProgramList[prg].fragmentShader = fs;
  ProgramList[prg].vertexShader = vs;
}

/**
 * @brief This function selects which vertex attributes should be interpolated during rasterization into fragment attributes.
 *
 * @param prg shader program
 * @param attrib id of attribute
 * @param type type of attribute
 */
void             GPU::setVS2FSType          (ProgramID prg,uint32_t attrib,AttributeType type){
  /// \todo tato funkce by měla zvolit typ vertex atributu, který je posílán z vertex shaderu do fragment shaderu.<br>
  /// V průběhu rasterizace vznikají fragment.<br>
  /// Fragment obsahují fragment atributy.<br>
  /// Tyto atributy obsahují interpolované hodnoty vertex atributů.<br>
  /// Tato funkce vybere jakého typu jsou tyto interpolované atributy.<br>
  /// Bez jakéhokoliv nastavení jsou atributy prázdne AttributeType::EMPTY<br>
  if(!isProgram(prg))
    return; 
  if(attrib > maxAttributes)
    return;

  ProgramList[prg].attributeType[attrib] = type; 
  
}

/**
 * @brief This function actives selected shader program
 *
 * @param prg shader program id
 */
void             GPU::useProgram            (ProgramID prg){
  /// \todo tato funkce by měla vybrat aktivní shader program.
  if(!isProgram(prg))
    return;
  ActiveProgramID = prg;
}

/**
 * @brief This function tests if selected shader program exists.
 *
 * @param prg shader program
 *VertexShaderVertexShader
 * @return true, if shader program "prg" exists.
 */
bool             GPU::isProgram             (ProgramID prg){
  /// \todo tato funkce by měla zjistit, zda daný program existuje.<br>
  /// Funkce vráti true, pokud program existuje.<br>
  return (prg < ProgramList.size() && ProgramList[prg].exist == true);
}

/**
 * @brief This function sets uniform value (1 float).
 *
 * @param prg shader program
 * @param uniformId id of uniform value (number of uniform values is stored in maxUniforms variable)
 * @param d value of uniform variable
 */
void             GPU::programUniform1f      (ProgramID prg,uint32_t uniformId,float     const&d){
  /// \todo tato funkce by měla nastavit uniformní proměnnou shader programu.<br>
  /// Parametr "prg" vybírá shader program.<br>
  /// Parametr "uniformId" vybírá uniformní proměnnou. Maximální počet uniformních proměnných je uložen v programné \link maxUniforms \endlink.<br>
  /// Parametr "d" obsahuje data (1 float).<br>
  if(!isProgram(prg))
    return;
  
  if(uniformId > maxUniforms)
    return;

  ProgramList[prg].uniforms.uniform[uniformId].v1 = d;

}

/**
 * @brief This function sets uniform value (2 float).
 *
 * @param prg shader program
 * @param uniformId id of uniform value (number of uniform values is stored in maxUniforms variable)
 * @param d value of uniform variable
 */
void             GPU::programUniform2f      (ProgramID prg,uint32_t uniformId,glm::vec2 const&d){
  /// \todo tato funkce dělá obdobnou věc jako funkce programUniform1f.<br>
  /// Místo 1 floatu nahrává 2 floaty.
  
  if(!isProgram(prg))
    return;
    
  if(uniformId > maxUniforms)
    return;

  ProgramList[prg].uniforms.uniform[uniformId].v2 = d;
}

/**
 * @brief This function sets uniform value (3 float).
 *
 * @param prg shader program
 * @param uniformId id of uniform value (number of uniform values is stored in maxUniforms variable)
 * @param d value of uniform variable
 */
void             GPU::programUniform3f      (ProgramID prg,uint32_t uniformId,glm::vec3 const&d){
  /// \todo tato funkce dělá obdobnou věc jako funkce programUniform1f.<br>
  /// Místo 1 floatu nahrává 3 floaty.
  
  if(!isProgram(prg))
    return;

  if(uniformId > maxUniforms)
    return;
  
  ProgramList[prg].uniforms.uniform[uniformId].v3 = d;
}

/**
 * @brief This function sets uniform value (4 float).
 *
 * @param prg shader program
 * @param uniformId id of uniform value (number of uniform values is stored in maxUniforms variable)
 * @param d value of uniform variable
 */
void             GPU::programUniform4f      (ProgramID prg,uint32_t uniformId,glm::vec4 const&d){
  /// \todo tato funkce dělá obdobnou věc jako funkce programUniform1f.<br>
  /// Místo 1 floatu nahrává 4 floaty.
  if(!isProgram(prg))
    return;

  if(uniformId > maxUniforms)
    return;  
  
  ProgramList[prg].uniforms.uniform[uniformId].v4 = d;
}

/**
 * @brief This function sets uniform value (4 float).
 *
 * @param prg shader program
 * @param uniformId id of uniform value (number of uniform values is stored in maxUniforms variable)
 * @param d value of uniform variable
 */
void             GPU::programUniformMatrix4f(ProgramID prg,uint32_t uniformId,glm::mat4 const&d){
  /// Místo 1 floatu nahrává matici 4x4 (16 floatů).
  /// \todo tato funkce dělá obdobnou věc jako funkce programUniform1f.<br>
  if(!isProgram(prg))
    return;
  
  if(uniformId > maxUniforms)
    return;

  ProgramList[prg].uniforms.uniform[uniformId].m4 = d;
}

/// @}

/** \addtogroup framebuffer_tasks 04. Implementace obslužných funkcí pro framebuffer
 * @{
 */

/**
 * @brief This function creates framebuffer on GPU.
 *
 * @param width width of framebuffer
 * @param height height of framebuffer
 */
void GPU::createFramebuffer      (uint32_t width,uint32_t height){
  /// \todo Tato funkce by měla alokovat framebuffer od daném rozlišení.<br>
  /// Framebuffer se skládá z barevného a hloukového bufferu.<br>
  /// Buffery obsahují width x height pixelů.<br>
  /// Barevný pixel je složen z 4 x uint8_t hodnot - to reprezentuje RGBA barvu.<br>
  /// Hloubkový pixel obsahuje 1 x float - to reprezentuje hloubku.<br>
  /// Nultý pixel framebufferu je vlevo dole.<br>

  FrameBuffer.depthBuffer = (float*) malloc(sizeof(float)*height*width);
  if (FrameBuffer.depthBuffer == NULL)
  {
    std::cerr << "Problem with allocating depth buffer" << std::endl;
    return;
  }

  FrameBuffer.colorBuffer = (uint8_t*) malloc(4*width*height);
  if(FrameBuffer.colorBuffer == NULL)
  {
    std::cerr << "Problem with allocating color buffer" << std::endl;
    free(FrameBuffer.depthBuffer);
    return;
  }

  FrameBuffer.height = height;
  FrameBuffer.width = width;

  for (int i = 0; i < FrameBuffer.width*FrameBuffer.height;i++)
  {    
    FrameBuffer.depthBuffer[i] = INFINITY;
  }
}

/**
 * @brief This function deletes framebuffer.
 */
void GPU::deleteFramebuffer      (){
  /// \todo tato funkce by měla dealokovat framebuffer.
  free(FrameBuffer.colorBuffer);
  free(FrameBuffer.depthBuffer);
  FrameBuffer.height = 0;
  FrameBuffer.width = 0;
}

/**
 * @brief This function resizes framebuffer.
 *
 * @param width new width of framebuffer
 * @param height new heght of framebuffer
 */
void     GPU::resizeFramebuffer(uint32_t width,uint32_t height){
  /// \todo Tato funkce by měla změnit velikost framebuffer.
  
  FrameBuffer.depthBuffer = (float*) realloc(FrameBuffer.depthBuffer,sizeof(float)*height*width);
  if (FrameBuffer.depthBuffer == NULL)
    return;
  

  FrameBuffer.colorBuffer = (uint8_t*) realloc(FrameBuffer.colorBuffer,4*width*height);
  if(FrameBuffer.colorBuffer == NULL)
  {
    free(FrameBuffer.depthBuffer);
    return;
  }

  FrameBuffer.height = height;
  FrameBuffer.width = width;
}

/**
 * @brief This function returns pointer to color buffer.
 *
 * @return pointer to color buffer
 */
uint8_t* GPU::getFramebufferColor  (){
  /// \todo Tato funkce by měla vrátit ukazatel na začátek barevného bufferu.<br>
  return FrameBuffer.colorBuffer;
}

/**
 * @brief This function returns pointer to depth buffer.
 *
 * @return pointer to dept buffer.
 */
float* GPU::getFramebufferDepth    (){
  /// \todo tato funkce by mla vrátit ukazatel na začátek hloubkového bufferu.<br>
  return FrameBuffer.depthBuffer;
}

/**
 * @brief This function returns width of framebuffer
 *
 * @return width of framebuffer
 */
uint32_t GPU::getFramebufferWidth (){
  /// \todo Tato funkce by měla vrátit šířku framebufferu.
  return FrameBuffer.width;
}

/**
 * @brief This function returns height of framebuffer.
 *
 * @return height of framebuffer
 */
uint32_t GPU::getFramebufferHeight(){
  /// \todo Tato funkce by měla vrátit výšku framebufferu.
  return FrameBuffer.height;
}

/// @}

/** \addtogroup draw_tasks 05. Implementace vykreslovacích funkcí
 * Bližší informace jsou uvedeny na hlavní stránce dokumentace.
 * @{
 */

/**
 * @brief This functino clears framebuffer.
 *
 * @param r red channel
 * @param g green channel
 * @param b blue channel
 * @param a alpha channel
 */
void            GPU::clear                 (float r,float g,float b,float a){
  /// \todo Tato funkce by měla vyčistit framebuffer.<br>
  /// Barevný buffer vyčistí na barvu podle parametrů r g b a (0 - nulová intenzita, 1 a větší - maximální intenzita).<br>
  /// (0,0,0) - černá barva, (1,1,1) - bílá barva.<br>
  /// Hloubkový buffer nastaví na takovou hodnotu, která umožní rasterizaci trojúhelníka, který leží v rámci pohledového tělesa.<br>
  /// Hloubka by měla být tedy větší než maximální hloubka v NDC (normalized device coordinates).<br>
  uint8_t red = r*255;
  uint8_t green = g*255;
  uint8_t blue = b*255;
  uint8_t alpha = a*255;

  for (int i = 0; i < FrameBuffer.width*FrameBuffer.height;i++)
  {    
    FrameBuffer.colorBuffer[(4*i)] = red;
    FrameBuffer.colorBuffer[(4*i+1)] = green;
    FrameBuffer.colorBuffer[(4*i+2)] = blue;
    FrameBuffer.colorBuffer[(4*i+3)] = alpha;

    FrameBuffer.depthBuffer[i] = INFINITY;
  }
}


void            GPU::_VertexPuller      (InVertex &inVertex,uint32_t iterator){

    void *data;

    //Creating VertexID
    if(VertexPullerList[ActiveVertexPullerID].indexType != IndexType::UINT16 && VertexPullerList[ActiveVertexPullerID].indexType != IndexType::UINT32 && VertexPullerList[ActiveVertexPullerID].indexType != IndexType::UINT8)
    {
      VertexBuffer.resize(VertexBuffer.size()+1);
      
      inVertex.gl_VertexID = VertexBufferID++;
      VertexBuffer.push_back(inVertex);
    
    }
    else
    {
      //BufferID with indexs
      BufferID bufferID = VertexPullerList[ActiveVertexPullerID].bufferID;
      uint64_t items = BufferList[bufferID].length;
    
      size_t size;
      switch (VertexPullerList[ActiveVertexPullerID].indexType)
      {
      case IndexType::UINT8:
        size = sizeof(uint8_t);
        break;
      case IndexType::UINT16:
        size = sizeof(uint16_t);
        break;
      case IndexType::UINT32:
        size = sizeof(uint32_t);
      default:
        break;
      }

      data = malloc(size);
      getBufferData(bufferID,size*iterator,size,data);

      //retype void* to numb pointer and dereference it.
      
      switch (VertexPullerList[ActiveVertexPullerID].indexType)
      {
      case IndexType::UINT8:
        inVertex.gl_VertexID = *((uint8_t*)(data));
      break;
      case IndexType::UINT16:
        inVertex.gl_VertexID = *((uint16_t*)(data));
        break;
      case IndexType::UINT32:
        inVertex.gl_VertexID = *((uint32_t*)(data));
      default:
        break;
      }

      if(inVertex.gl_VertexID == VertexBuffer.size())
      {
        VertexBuffer.resize(VertexBuffer.size()+1);
        VertexBuffer.push_back(inVertex);
      }

      free(data);
    }
}

void            GPU::_VertexProcessor      (OutVertex &outVertex,InVertex &inVertex, uint32_t iterator){
  void *data;
  //reading heads and filling vertex attributes
  for(int i = 0; i < maxAttributes;i++)
  {
    //If is current reading head actvive
    if(VertexPullerList[ActiveVertexPullerID].VP_Heads[i].enabled == true)
    {
        //Malloc for pointer
      data = malloc(sizeof(float)*(uint)VertexPullerList[ActiveVertexPullerID].VP_Heads[i].attributeType);
      
      if(data == NULL)
      {
        std::cerr << ">>>function drawTriangles_VS: Vertex shader problem with allocating a memory" << std::endl;
        return;
      }  
      //getBufferData(bufferID,offset+(stride*inVertex.gl_VertexID),size,data);
      getBufferData(VertexPullerList[ActiveVertexPullerID].VP_Heads[i].bufferID,                                          
                    VertexPullerList[ActiveVertexPullerID].VP_Heads[i].offset+(VertexPullerList[ActiveVertexPullerID].VP_Heads[i].stride*inVertex.gl_VertexID),
                    sizeof(float)*(uint)VertexPullerList[ActiveVertexPullerID].VP_Heads[i].attributeType,
                    data);
      
      switch (VertexPullerList[ActiveVertexPullerID].VP_Heads[i].attributeType)
      {
        case AttributeType::FLOAT:
            inVertex.attributes[i].v1 = *((float*)data);
          break;
        
        case AttributeType::VEC2:
            inVertex.attributes[i].v2 = *((glm::vec2*)data);
          break;

        case AttributeType::VEC3:
            inVertex.attributes[i].v3 = *((glm::vec3*)data); 
          break;
        
        case AttributeType::VEC4:
            inVertex.attributes[i].v4 = *((glm::vec4*)data);
          break;
      }
      free(data);          
      
    }
  }
  ProgramList[ActiveProgramID].vertexShader(outVertex,inVertex,ProgramList[ActiveProgramID].uniforms);
}
  
void            GPU::_PrimitiveAssembly    (std::vector<Triangle_s> &Triangles,uint32_t  nofTriangles)
{    
  for(uint32_t i = 0; i < nofTriangles;i++)
  {
    Triangle_s triangle;
    for(uint32_t j = 0; j < 3; j++)
    {
      OutVertex outVertex;
      outVertex.gl_Position = {0,0,0,0};
      InVertex inVertex; 
      _VertexPuller(inVertex,i*3+j);
      _VertexProcessor(outVertex, inVertex,i*3+j);
      triangle.vertex[j] = outVertex;
    }
    Triangles.push_back(triangle);
  }
}

void            GPU::_PerspectiveDivision    (Triangle_s &triangle){  
  for(uint32_t i = 0; i < 3; i++)
  {
    triangle.vertex[i].gl_Position.w = 1/triangle.vertex[i].gl_Position.w; 
    triangle.vertex[i].gl_Position.x *= triangle.vertex[i].gl_Position.w;
    triangle.vertex[i].gl_Position.y *= triangle.vertex[i].gl_Position.w;
    triangle.vertex[i].gl_Position.z *= triangle.vertex[i].gl_Position.w;
  }
}


void            GPU::_ViewPortTransform     (Triangle_s &triangle){
  for(uint32_t i = 0; i < 3; i++)
  {
    triangle.vertex[i].gl_Position.x = (triangle.vertex[i].gl_Position.x + 1)*FrameBuffer.width/2;
    triangle.vertex[i].gl_Position.y = (triangle.vertex[i].gl_Position.y + 1)*FrameBuffer.height/2;
  }
}

void            GPU::_FragmentProcessor    (InFragment &inFragment){

  OutFragment outFragment;
  outFragment.gl_FragColor = {0,0,0,0};
  
  ProgramList[ActiveProgramID].fragmentShader(outFragment,inFragment,ProgramList[ActiveProgramID].uniforms);
  
  //read color rgba from outfragment and save it to buffer
  uint8_t* ColorBuffer = getFramebufferColor();
  float* DepthBuffer = getFramebufferDepth();
  int x = (int)floor(inFragment.gl_FragCoord.x);
  int y = (int)floor(inFragment.gl_FragCoord.y);
  
  //depth test
 // std::cerr << "Depth buffer: " << DepthBuffer[(x+y*FrameBuffer.width)] << " z: " << inFragment.gl_FragCoord.z << std::endl;
  if(DepthBuffer[(x+y*FrameBuffer.width)] > inFragment.gl_FragCoord.z)
  {
    ColorBuffer[(x*4+y*4*FrameBuffer.width)] = (uint8_t)floor(outFragment.gl_FragColor.r*255);
    ColorBuffer[(x*4+y*4*FrameBuffer.width+1)] = (uint8_t)floor(outFragment.gl_FragColor.g*255);
    ColorBuffer[(x*4+y*4*FrameBuffer.width+2)] = (uint8_t)floor(outFragment.gl_FragColor.b*255);
    ColorBuffer[(x*4+y*4*FrameBuffer.width+3)] = (uint8_t)floor(outFragment.gl_FragColor.a*255);
    DepthBuffer[(x+y*FrameBuffer.width)] = inFragment.gl_FragCoord.z;
  }
}


void      GPU::_Clipping2V            (Triangle_s &triangle, bool *ClipVertex){

  float t[2];
  int frontVertex;
  for(int i = 0; i < 3;i++)
  {
    if(ClipVertex[i] == false)
      {
        frontVertex = i;
        break;
      }
  }

  //calculate first t
  t[0] = (-1*triangle.vertex[frontVertex].gl_Position.w - triangle.vertex[frontVertex].gl_Position.z)/
       (triangle.vertex[(frontVertex+1)%3].gl_Position.w - triangle.vertex[frontVertex].gl_Position.w + triangle.vertex[(frontVertex+1)%3].gl_Position.z - triangle.vertex[frontVertex].gl_Position.z);

  //calculate second t  
  t[1] = (-1*triangle.vertex[frontVertex].gl_Position.w - triangle.vertex[frontVertex].gl_Position.z)/
       (triangle.vertex[(frontVertex+2)%3].gl_Position.w - triangle.vertex[frontVertex].gl_Position.w + triangle.vertex[(frontVertex+2)%3].gl_Position.z - triangle.vertex[frontVertex].gl_Position.z);

  for(int i = 0; i < 2;i++)
  {
    //Iterating over positions x,y,z,w
    for(int k = 0; k < 4; k++)
    {
      triangle.vertex[(frontVertex+1 + i)%3].gl_Position[k] = triangle.vertex[frontVertex].gl_Position[k] + t[i]*(triangle.vertex[(frontVertex+1 +i)%3].gl_Position[k] - triangle.vertex[frontVertex].gl_Position[k]);
    }

    //calculate same way attributes
    for(int j = 0; j < maxAttributes;j++)
    {
        switch (ProgramList[ActiveProgramID].attributeType[j])
        {
        case AttributeType::EMPTY:
          continue;
          break;
        
        case AttributeType::FLOAT:
          triangle.vertex[(frontVertex+1 + i)%3].attributes[j].v1 = triangle.vertex[frontVertex].attributes[j].v1 + t[i]*(triangle.vertex[(frontVertex+1 + i)%3].attributes[j].v1 - triangle.vertex[frontVertex].attributes[j].v1);
          break;

        case AttributeType::VEC2:
          
          triangle.vertex[(frontVertex+1 + i)%3].attributes[j].v2 = triangle.vertex[frontVertex].attributes[j].v2 + t[i]*(triangle.vertex[(frontVertex+1 + i)%3].attributes[j].v2 - triangle.vertex[frontVertex].attributes[j].v2);
          break;

        case AttributeType::VEC3:
          
          triangle.vertex[(frontVertex+1 + i)%3].attributes[j].v3 = triangle.vertex[frontVertex].attributes[j].v3 + t[i]*(triangle.vertex[(frontVertex+1 + i)%3].attributes[j].v3 - triangle.vertex[frontVertex].attributes[j].v3);
          break;
        
        case AttributeType::VEC4:
          
          triangle.vertex[(frontVertex+1 + i)%3].attributes[j].v4 = triangle.vertex[frontVertex].attributes[j].v4 + t[i]*(triangle.vertex[(frontVertex+1 + i)%3].attributes[j].v4 - triangle.vertex[frontVertex].attributes[j].v4);
          break;
        }
    }
  }
}

void      GPU::_Clipping1V            (Triangle_s &triangle,bool *ClipVertex){


  float t[2];
  int backVertex;
  for(int i = 0; i < 3;i++)
  {
    if(ClipVertex[i])
      {
        backVertex = i;
        break;
      }
  }

  //calculate first t
  t[0] = (-1*triangle.vertex[backVertex].gl_Position.w - triangle.vertex[backVertex].gl_Position.z)/
       (triangle.vertex[(backVertex+1)%3].gl_Position.w - triangle.vertex[backVertex].gl_Position.w + triangle.vertex[(backVertex+1)%3].gl_Position.z - triangle.vertex[backVertex].gl_Position.z);

  //calculate second t  
  t[1] = (-1*triangle.vertex[backVertex].gl_Position.w - triangle.vertex[backVertex].gl_Position.z)/
       (triangle.vertex[(backVertex+2)%3].gl_Position.w - triangle.vertex[backVertex].gl_Position.w + triangle.vertex[(backVertex+2)%3].gl_Position.z - triangle.vertex[backVertex].gl_Position.z);


  OutVertex Vx[2];
  for(int i = 0; i < 2; i++)
  {
    //Calculating last point to 1st triangle
    for(int k = 0; k < 4; k++)
    {
      Vx[i].gl_Position[k] = triangle.vertex[backVertex].gl_Position[k] + t[i]*(triangle.vertex[(backVertex+1+i)%3].gl_Position[k] - triangle.vertex[backVertex].gl_Position[k]);
    }

    for(int j = 0; j < maxAttributes;j++)
    { 
        switch (ProgramList[ActiveProgramID].attributeType[j])
        {
        case AttributeType::EMPTY:
          continue;
          break;
        
        case AttributeType::FLOAT:
          Vx[i].attributes[j].v1 = triangle.vertex[backVertex].attributes[j].v1 + t[i]*(triangle.vertex[(backVertex + 1+i)%3].attributes[j].v1 - triangle.vertex[backVertex].attributes[j].v1);
          break;

        case AttributeType::VEC2:
          
          Vx[i].attributes[j].v2 = triangle.vertex[backVertex].attributes[j].v2 + t[i]*(triangle.vertex[(backVertex + 1+i)%3].attributes[j].v2 - triangle.vertex[backVertex].attributes[j].v2);
          break;

        case AttributeType::VEC3:
          
          Vx[i].attributes[j].v3 = triangle.vertex[backVertex].attributes[j].v3 + t[i]*(triangle.vertex[(backVertex + 1+i)%3].attributes[j].v3 - triangle.vertex[backVertex].attributes[j].v3);
          break;
        
        case AttributeType::VEC4:
          
          Vx[i].attributes[j].v4 = triangle.vertex[backVertex].attributes[j].v4 + t[i]*(triangle.vertex[(backVertex + 1+i)%3].attributes[j].v4 - triangle.vertex[backVertex].attributes[j].v4);
          break;
        }
    }
  }

  //Culling  
  Triangle_s SecondTriangle;

  SecondTriangle.vertex[0] = Vx[1]; 
  SecondTriangle.vertex[1] = triangle.vertex[(backVertex+2)%3];
  SecondTriangle.vertex[2] = triangle.vertex[(backVertex+1)%3];


  triangle.vertex[backVertex] = Vx[0];
  triangle.vertex[(backVertex+2)%3] = Vx[1];
  
  _PerspectiveDivision(SecondTriangle);
  _ViewPortTransform(SecondTriangle);  
  _Rasterize(SecondTriangle);

}



int            GPU::_Clipping             (Triangle_s &triangle){
  
  int counter = 0;
  bool ClipVertex[3] = {false};
  
  //calculate if vertex will be clipped
  for(int i = 0; i < 3;i++)
  {  
    float negW = triangle.vertex[i].gl_Position.w * -1;
    if(negW > triangle.vertex[i].gl_Position.z)
      {
        counter++;
        ClipVertex[i] = true;
      }
  }


  //If all vertexes are behind nearplane drop a triangle
  if(counter == 3)
  {
    return 1;
  }
  if(counter == 2)
  {
    _Clipping2V(triangle,ClipVertex);
  }
  if(counter == 1)
  {
    _Clipping1V(triangle,ClipVertex);
  }
  return 0;
}


float      GPU::_FindMin               (float a, float b, float c){
  float temp = a;
  if(temp > b)
    temp = b;
  if(temp > c)
    temp = c;
  return temp;
}


float      GPU::_FindMax               (float a, float b, float c){
  float temp = a;
  if(temp < b)
    temp = b;
  if(temp < c)
    temp = c;
  return temp;

}

void      GPU::_Rasterize         (Triangle_s &triangle){

  /* bounding box */

  OutVertex V0 = triangle.vertex[0];
  OutVertex V1 = triangle.vertex[1];
  OutVertex V2 = triangle.vertex[2];
  
  float minY = _FindMin(V0.gl_Position.y,
                        V1.gl_Position.y,
                        V2.gl_Position.y);
  
  float maxY = _FindMax(V0.gl_Position.y,
                        V1.gl_Position.y,
                        V2.gl_Position.y);
  
  float minX = _FindMin(V0.gl_Position.x,
                        V1.gl_Position.x,
                        V2.gl_Position.x);
  
  float maxX = _FindMax(V0.gl_Position.x,
                        V1.gl_Position.x,
                        V2.gl_Position.x);

  
  if(minY < 0 || minX < 0 || maxY > FrameBuffer.height || maxX > FrameBuffer.width || 
    !std::isfinite(minY) || !std::isfinite(maxY) || !std::isfinite(minX) || !std::isfinite(maxX))
    return;

//https://stackoverflow.com/questions/24441631/how-exactly-does-opengl-do-perspectively-correct-linear-interpolation
  // precompute the affine transform from fragment coordinates to barycentric coordinates
    const float denom = 1/((V0.gl_Position.x - V2.gl_Position.x)*(V1.gl_Position.y - V0.gl_Position.y) - (V0.gl_Position.x - V1.gl_Position.x)*(V2.gl_Position.y - V0.gl_Position.y));

    const glm::vec3 barycentric_d0 = {denom*(V1.gl_Position.y - V2.gl_Position.y),
                                      denom*(V2.gl_Position.y - V0.gl_Position.y),
                                      denom*(V0.gl_Position.y - V1.gl_Position.y)};

    const glm::vec3 barycentric_d1 = {denom*(V2.gl_Position.x - V1.gl_Position.x),
                                      denom*(V0.gl_Position.x - V2.gl_Position.x),
                                      denom*(V1.gl_Position.x - V0.gl_Position.x)};

    const glm::vec3 barycentric_0 = {denom * (V1.gl_Position.x*V2.gl_Position.y - V2.gl_Position.x*V1.gl_Position.y),
                                     denom * (V2.gl_Position.x*V0.gl_Position.y - V0.gl_Position.x*V2.gl_Position.y),
                                     denom * (V0.gl_Position.x*V1.gl_Position.y - V1.gl_Position.x*V0.gl_Position.y)};

  /* searching in bounding box */

  for(int y = minY; y <= maxY;y++)
  {
    for(int x = minX; x <= maxX;x++)
    {
      InFragment fragment;
      fragment.gl_FragCoord.x = x+0.5f;
      fragment.gl_FragCoord.y = y+0.5f;

      // fragment barycentric coordinates in window coordinates

      const glm::vec3 barycentric = {(fragment.gl_FragCoord.x*barycentric_d0[0] + fragment.gl_FragCoord.y*barycentric_d1[0] + barycentric_0[0]),
                                     (fragment.gl_FragCoord.x*barycentric_d0[1] + fragment.gl_FragCoord.y*barycentric_d1[1] + barycentric_0[1]),
                                     (fragment.gl_FragCoord.x*barycentric_d0[2] + fragment.gl_FragCoord.y*barycentric_d1[2] + barycentric_0[2])};


        // discard fragment outside the triangle.
        if(barycentric[0] < 0 || barycentric[1] < 0 || barycentric[2] < 0)
            continue;

        // interpolate inverse w linearly
        fragment.gl_FragCoord.w = glm::dot(barycentric, glm::vec3(V0.gl_Position.w, V1.gl_Position.w, V2.gl_Position.w));


        // convert to perspective correct (clip-space) barycentric
        const glm::vec3 perspective = {(1/fragment.gl_FragCoord.w*barycentric[0]*V0.gl_Position.w),
                                       (1/fragment.gl_FragCoord.w*barycentric[1]*V1.gl_Position.w),
                                       (1/fragment.gl_FragCoord.w*barycentric[2]*V2.gl_Position.w)};

          fragment.gl_FragCoord.z = glm::dot(perspective,glm::vec3(V0.gl_Position.z,V1.gl_Position.z,V2.gl_Position.z));


        //From perspective vector calculate perspective correct interpolation in every attribute
         for(int i = 0; i < maxAttributes;i++)
        {
          AttributeType atrType = ProgramList[ActiveProgramID].attributeType[i];
          
          if(atrType == AttributeType::EMPTY)
            continue;
         
          else if(atrType == AttributeType::FLOAT)
            fragment.attributes[i].v1 = glm::dot(perspective,glm::vec3(V0.attributes[i].v1,V1.attributes[i].v1,V2.attributes[i].v1)); 
          
          else if(atrType == AttributeType::VEC2)
            fragment.attributes[i].v2 = glm::vec2{(glm::dot(perspective,glm::vec3(V0.attributes[i].v2[0],V1.attributes[i].v2[0],V2.attributes[i].v2[0]))),
                                                  (glm::dot(perspective,glm::vec3(V0.attributes[i].v2[1],V1.attributes[i].v2[1],V2.attributes[i].v2[1])))};

          else if(atrType == AttributeType::VEC3)
            fragment.attributes[i].v3 = glm::vec3{(glm::dot(perspective,glm::vec3(V0.attributes[i].v3[0],V1.attributes[i].v3[0],V2.attributes[i].v3[0]))),
                                                  (glm::dot(perspective,glm::vec3(V0.attributes[i].v3[1],V1.attributes[i].v3[1],V2.attributes[i].v3[1]))),
                                                  (glm::dot(perspective,glm::vec3(V0.attributes[i].v3[2],V1.attributes[i].v3[2],V2.attributes[i].v3[2])))};

          else if(atrType == AttributeType::VEC4)
            fragment.attributes[i].v4 = glm::vec4{(glm::dot(perspective,glm::vec3(V0.attributes[i].v4[0],V1.attributes[i].v4[0],V2.attributes[i].v4[0]))),
                                                  (glm::dot(perspective,glm::vec3(V0.attributes[i].v4[1],V1.attributes[i].v4[1],V2.attributes[i].v4[1]))),
                                                  (glm::dot(perspective,glm::vec3(V0.attributes[i].v4[2],V1.attributes[i].v4[2],V2.attributes[i].v4[2]))),
                                                  (glm::dot(perspective,glm::vec3(V0.attributes[i].v4[3],V1.attributes[i].v4[3],V2.attributes[i].v4[3])))};
        }
        _FragmentProcessor(fragment);
    }
  }
}


void            GPU::drawTriangles         (uint32_t  nofVertices){
  /// \todo Tato funkce vykreslí trojúhelníky podle daného nastavení.<br>
  /// Vrcholy se budou vybírat podle nastavení z aktivního vertex pulleru (pomocí bindVertexPuller).<br>
  /// Vertex shader a fragment shader se zvolí podle aktivního shader programu (pomocí useProgram).<br>
  /// Parametr "nofVertices" obsahuje počet vrcholů, který by se měl vykreslit (3 pro jeden trojúhelník).<br>

    std::vector<Triangle_s> Triangles;
    //reset vertex invocation counter
    VertexBufferID = 0;
    
    uint32_t nofTriangles = nofVertices/3;
    Triangles.reserve(nofTriangles);
    _PrimitiveAssembly(Triangles,nofTriangles);
    
    for (int i = 0; i < Triangles.size(); i++)
    {
      if(_Clipping(Triangles[i]) == 1)
        continue;
           
      _PerspectiveDivision(Triangles[i]);
      _ViewPortTransform(Triangles[i]);

      _Rasterize(Triangles[i]);
    }

    Triangles.clear();
}

/// @}
