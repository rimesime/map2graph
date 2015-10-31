# map2graph
Generating graph structure from map.

How to:
- place grayscaled image in folder called map.png
- >make
- >./map2graph <number of iterations>
- writes/overwrites map_graph.png

What works now: 
- marking paths between walls, giving image with white path
- by applying kernel images to suitable pixels (eroding walls)
- stops after given iterations or if no change in last iteration

Future Work:
- make it (more) efficient (threads, code optimizations, ...)
- use http://arohatgi.info/WebPlotDigitizer/ to extract points?
- or directly walk on pixel paths and extract corners by looking at angle differences
