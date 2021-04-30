/*!
 * @file
 * @brief This file contains implementation of gpu
 *
 * @author Tomáš Milet, imilet@fit.vutbr.cz
 */

#include <student/gpu.hpp>


/*
 * @brief This function ll count index of triangle vertex
 *
 * 
 */
uint32_t computeVertexID(VertexArray const&vao,uint32_t shaderInvocation)
{
  if(vao.indexBuffer == nullptr)return shaderInvocation;

  if(vao.indexType == IndexType::UINT32)
  {
    uint32_t*ind = (uint32_t*)vao.indexBuffer;
    return ind[shaderInvocation];
  }
  
  if(vao.indexType == IndexType::UINT16)
  {
    uint16_t*ind = (uint16_t*)vao.indexBuffer;
    return ind[shaderInvocation];
  }
  
  if(vao.indexType == IndexType::UINT8)
  {
    uint8_t*ind = (uint8_t*)vao.indexBuffer;
    return ind[shaderInvocation];
  }
  return shaderInvocation;
}


/**
 *  Sends  ctx.vao.vertexAttrib[0].bufferData to Invertex.atrrbutes[j]
 * 
 */
void vertex_to_vertex_shader(InVertex &inVertex, GPUContext &ctx)
{
  for(uint32_t j = 0; j < maxAttributes; j++)
  {
    auto const &possition = ctx.vao.vertexAttrib[j];
    
    if(!possition.bufferData)
      continue;

    if(possition.type == AttributeType::FLOAT)
      inVertex.attributes[j].v1 = *(float*)(((uint8_t*)possition.bufferData) + possition.offset + possition.stride * inVertex.gl_VertexID);
    if(possition.type == AttributeType::VEC2)
      inVertex.attributes[j].v2 = *(glm::vec2*)(((uint8_t*)possition.bufferData) + possition.offset + possition.stride * inVertex.gl_VertexID);
    if(possition.type == AttributeType::VEC3)
      inVertex.attributes[j].v3 = *(glm::vec3*)(((uint8_t*)possition.bufferData) + possition.offset + possition.stride * inVertex.gl_VertexID);
    if(possition.type == AttributeType::VEC4)
      inVertex.attributes[j].v4 = *(glm::vec4*)(((uint8_t*)possition.bufferData) + possition.offset + possition.stride * inVertex.gl_VertexID);
  }
}

//! [drawTrianglesImpl]
void drawTrianglesImpl(GPUContext &ctx,uint32_t nofVertices)
{
  (void)ctx;
  (void)nofVertices;
  /// \todo Tato funkce vykreslí trojúhelníky podle daného nastavení.<br>
  /// ctx obsahuje aktuální stav grafické karty.
  /// Parametr "nofVertices" obsahuje počet vrcholů, který by se měl vykreslit (3 pro jeden trojúhelník).<br>
  /// Bližší informace jsou uvedeny na hlavní stránce dokumentace.

  InVertex  inVertex; // vrchol, co vstupuje do vertex shader
  OutVertex outVertex;// vrchol, co leze ven z vertex shaderu 

  //if(ctx.vao.vertexAttrib->type == AttributeType::FLOAT)
  //{
//  }  
  
  for(int i = 0; i < nofVertices; i++)
  {
  
      //smyčka přes vrcholy
    inVertex.gl_VertexID = computeVertexID(ctx.vao, i );

    if (ctx.vao.vertexAttrib->type == AttributeType::FLOAT) 
      vertex_to_vertex_shader(inVertex, ctx);

    ctx.prg.vertexShader(outVertex, inVertex, ctx.prg.uniforms);

    

  }
  //if()


}
//! [drawTrianglesImpl]

/**
 * @brief This function reads color from texture.
 *
 * @param texture texture
 * @param uv uv coordinates
 *
 * @return color 4 floats
 */
glm::vec4 read_texture(Texture const&texture,glm::vec2 uv){
  if(!texture.data)return glm::vec4(0.f);
  auto uv1 = glm::fract(uv);
  auto uv2 = uv1*glm::vec2(texture.width-1,texture.height-1)+0.5f;
  auto pix = glm::uvec2(uv2);
  //auto t   = glm::fract(uv2);
  glm::vec4 color = glm::vec4(0.f,0.f,0.f,1.f);
  for(uint32_t c=0;c<texture.channels;++c)
    color[c] = texture.data[(pix.y*texture.width+pix.x)*texture.channels+c]/255.f;
  return color;
}

/**
 * @brief This function clears framebuffer.
 *
 * @param ctx GPUContext
 * @param r red channel
 * @param g green channel
 * @param b blue channel
 * @param a alpha channel
 */
void clear(GPUContext&ctx,float r,float g,float b,float a){
  auto&frame = ctx.frame;
  auto const nofPixels = frame.width * frame.height;
  for(size_t i=0;i<nofPixels;++i){
    frame.depth[i] = 10e10f;
    frame.color[i*4+0] = static_cast<uint8_t>(glm::min(r*255.f,255.f));
    frame.color[i*4+1] = static_cast<uint8_t>(glm::min(g*255.f,255.f));
    frame.color[i*4+2] = static_cast<uint8_t>(glm::min(b*255.f,255.f));
    frame.color[i*4+3] = static_cast<uint8_t>(glm::min(a*255.f,255.f));
  }
}

