set glslcExePath=E:\dev\ExternalSources\VulkanSDK\1.3.236.0\Bin/glslc.exe
%glslcExePath% shader.vert -o vert.spv
%glslcExePath% shader.frag -o frag.spv
%glslcExePath% staticTile.vert -o staticTileVert.spv
%glslcExePath% staticTile.frag -o staticTileFrag.spv
%glslcExePath% player.vert -o playerVert.spv
%glslcExePath% player.frag -o playerFrag.spv
pause