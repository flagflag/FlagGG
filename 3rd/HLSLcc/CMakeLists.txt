set (TARGET_NAME hlslcc)

set(CMAKE_CXX_STANDARD 14)

file(GLOB HLSLCC_HDRS "${CMAKE_CURRENT_SOURCE_DIR}/include/*")
set(HLSLCC_SRC
		src/ControlFlowGraph.cpp
		src/ControlFlowGraphUtils.cpp
		src/DataTypeAnalysis.cpp
		src/Declaration.cpp
		src/decode.cpp
		src/HLSLcc.cpp
		src/HLSLccToolkit.cpp
		src/HLSLCrossCompilerContext.cpp
		src/Instruction.cpp
		src/LoopTransform.cpp
		src/Operand.cpp
		src/reflect.cpp
		src/Shader.cpp
		src/ShaderInfo.cpp
		src/toGLSL.cpp
		src/toGLSLDeclaration.cpp
		src/toGLSLInstruction.cpp
		src/toGLSLOperand.cpp
		src/toMetal.cpp
		src/toMetalDeclaration.cpp
		src/toMetalInstruction.cpp
		src/toMetalOperand.cpp
		src/UseDefineChains.cpp
		src/cbstring/bsafe.c 
		src/cbstring/bstraux.c
		src/cbstring/bstrlib.c)


add_library(hlslcc ${HLSLCC_SRC} ${HLSLCC_HDRS})

target_include_directories(hlslcc
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
    PRIVATE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/cbstring>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/internal_includes>)

set_target_properties( hlslcc PROPERTIES FOLDER "3rd" )

