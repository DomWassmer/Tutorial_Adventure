set glslcExePath=C:/VulkanSDK/$(Version)/Bin/glslc.exe
%glslcExePath% shader.vert -o vert.spv
%glslcExePath% shader.frag -o frag.spv
%glslcExePath% staticTile.vert -o staticTileVert.spv
%glslcExePath% staticTile.frag -o staticTileFrag.spv
pause