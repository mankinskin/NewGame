the one and only project I am working on

The vision of this game made me start programming out of nowhere (i was only playing games before) and it turns out that i love it.

I dont expect to get this game "done"(well playable) before 2020 and i dont know if it will ever be as good as i believe it will be, but I will just keep working on it and keep learning with it.

-----------------------------------------------------------------------------------------------------------------------------------
TO-DO:
-	
GUI
	- [ ]Elements: groups of primitives(Quads, Lines, ...) associated as one element
		- [ ]member primitives should be initialized at element initialization -> makes things easier and safer
		- [ ]find a way to distribute changes to the element to all its members
	- [ ]Color
		- [ ]dynamically update color buffer
		- [ ]reference colors by index
			- allows reuse
	- [ ]Lines
		- [ ]one line can be represented with 2 2D vertices (eg 4 floats) 
		- [ ]lines need color
	- [ ]give elements a depth order: 
		- [ ]elements should be rendered on top of each other in a more easily customizable order(primitives also)
	- [ ]Masking
		- [ ]masking is needed in various places:
			1. button masking: button indices should only be rendered if the button is imidiately visible (easy)
			2. some elements should only be visible at all at specified areas of the screen (to enable scrolling in windows/widgets)
	- [ ]quad textures
		- [ ]quads should be able to use textures
		- [ ]one special way of displaying a texture is needed to display fancier widget borders.
			x---x--------x---x
			|cor| border |cor|
			x---x--------x---x
			|bor| window |bor|
			|der| window |der|
			x---x--------x---x
			|cor| border |cor|
			x---x--------x---x
			- the quad is split up into/actually starts out as 9 quads. 
			- the UV coordinates into the textures are used to map the texture for the borders & corners onto the quads

Lighting
	- [ ]Shadow Volumes
		- should be computed on the GPU using compute shaders
	- [ ]Spotlights
	- [ ]Materials
Models
	- [ ]Mesh Textures
	- [ ]Model Rigs
		- [ ]this is a big one
		- [ ]a node hierachy(armature) transforms vertices of a mesh
			- [ ]each node has a group of vertices it transform by its own transformation * the vertex weight
Physics
	- [ ]Boundingboxes
		- [ ]each mesh can have x bounding boxes
		- [ ]boundingboxes can transform a model skeleton

AI
	- 

---------------------------------------------------------------------------------------------------------------------------------------