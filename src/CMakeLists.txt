# others
add_subdirectory(Tracer)

# "workers"
add_subdirectory(shared)
add_subdirectory(posInputWorker)
#add_subdirectory(mapLoadWorker)
#add_subdirectory(horizonGenerator)

# main executable
add_subdirectory(microHorizonApp)