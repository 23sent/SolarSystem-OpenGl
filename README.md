# SolarSystem-OpenGl
A Solar System model with legacy opengl. Computer Graphics course assignment.

# Interactive Functions
## Keyboard
	l -> Enable/Disable Lighting
	t -> Enable/Disble Texture
	o -> Enable/Disable drawOrbit
	- -> decrease time speed
	+ -> increase time speed
	0 -> reset camera rotation/position
	w, s -> change X axis rotation of camera around sun
	a, d -> change Z axis rotation of camera around sun
	
## Mouse
	left click and mouse motion-> change view angle
	right click -> glut menu, to change light color, speed or texture
	scroll up/down -> zoom in/out to sun (move in z axis)

# Distance and Sizes
Orbitals distances is life-size as AU (astronomical unit). Planets sizes also proportional.
	
But the sun's size a bit tricky. I could not do sun as big as in life. 
Because when I tried enlarging sun, other planets was absorbed by sun. 
Then I tried enlarge orbits with sun, but this time the planets became small and impossible to see.
So, I scaled sun's size different from other distances.

# Orbital and Sphere
Celestial bodies has elliptic orbitals like real life, and sun at one of the two focus. 
I preferred Icospheres to render planets.

# Sources
Texture images from: https://www.solarsystemscope.com/textures/

#-
freeglut	2.8.1-1.mp
glew 		1.11.0
