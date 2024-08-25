hunter_config(
    OpenCV
    VERSION "${HUNTER_OpenCV_VERSION}"
    CMAKE_ARGS 
    	WITH_IPP=OFF
        WITH_PROTOBUF=OFF
        WITH_JASPER=OFF
)

hunter_config(
    LLVM
    VERSION "${HUNTER_LLVM_VERSION}"
    CMAKE_ARGS 
        LLVM_ENABLE_DUMP=ON
)
