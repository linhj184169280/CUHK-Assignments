panorama rdWarp ./mountains/1.tga   ./mountains/goodimg_1.tga   595 0 0
panorama rdWarp ./mountains/2.tga   ./mountains/goodimg_2.tga   595 0 0
panorama rdWarp ./mountains/3.tga   ./mountains/goodimg_3.tga   595 0 0
panorama rdWarp ./mountains/4.tga   ./mountains/goodimg_4.tga   595 0 0
panorama rdWarp ./mountains/5.tga   ./mountains/goodimg_5.tga   595 0 0
panorama rdWarp ./mountains/6.tga   ./mountains/goodimg_6.tga   595 0 0

panorama alignAll ./mountains.txt ./mountains/orientations.txt 20  595  100  10 8 sift

panorama blendAll ./mountains/orientations.txt ./mountains/outimg.tga  595  200