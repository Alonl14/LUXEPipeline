#
# Include the geometry component
# to the pipeline core
#

# Geant4 Plugin is required
# for the geometry
find_package(
    Acts
    COMPONENTS
    PluginGeant4
    REQUIRED)

# Includes for Acts are propagated
# through the library linking
target_include_directories(
    ActsLUXEPipelineCore
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)

# Sources
target_sources(
    ActsLUXEPipelineCore
    PRIVATE
    src/LUXEGeometry.cpp
    src/LUXEGeometryIdGenerator.cpp
    src/E320Geometry.cpp
    src/E320GeometryIdGenerator.cpp
)

# For now we assume that the pipeline 
# always uses the Geant4 plugin
target_link_libraries(
    ActsLUXEPipelineCore
    PUBLIC
    ActsCore
    ActsPluginGeant4
)
      