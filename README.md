Hello, and welcome to the world of textiles! 

I’m Ria, I’m Afia - and this is our final project rendering textiles!  

### Mass cloth

We chose to work in openGL with our main focus on implementing a mass-spring cloth simulation system to imitate the movement of cloth in realtime, and we’ll walk through how we implemented it. At a high level, mass cloth simulation involves simulating physics so that we can render a realistic looking cloth by simulating forces such as gravity and wind. 

### Implementation

We started by defining a cloth, with particles and springs modelled as structs within. The cloth’s particle positions were hardcoded in world space, and accessible with getters and setters to be able to update position on different timed events. The particle stored its position, acceleration, and old positon, which springs stored the particles they are connected to, the rest length between them, and stiffness. 
  class Particle {
    Vector3 position;
    Vector3 velocity;
    Vector3 acceleration;
}

step2 : render the cloth with positions & normals in the same winding order as polygon’s vertexData to comply with openGL’s render pipeline. So we wrote functions that modified the vbo data based on time intervals of update calls, which allowed the cloth to render on screen.

step 3: add forces : we implemented this on a key press but essentially we first added the acceleration force due to gravity, and then looped throuhg every spring calculating the force exerted on it’s 2 adjacent particles, that stops the cloth from falling infinitely, and defines it’s properties as a viscous object.

we then computed the new position based on the next time interval using verlet’s integration, which requires storing the old position, and with each update call, calculating the new position of the point after the time interval, and storing the previous position. 

Add wind effect by using a graphed sine wave where the x-axis was time and the y is position, so on an given interval, a certain row of vertices would move to achieve the wind/wave effect.

### Design & Flow Check

The given pieces of the this project will fit together using a structure similar to Projects 5&6 - we will have a 2d polygon to represent a cloth we want to render

The scenefiles we describe with json will contain the shape, and image files that will describe the textures we want to render. We will define and describe the mesh object as masses.

We will then have a VAO / VBO class that will create and bind VBOs & VAOs, fill the data on intitliaze or a parameter change, and then draw arrays, again same as Project 5. This will handle functionality to be able to render multiple textures on screen with multiple VBOs& VAOs. 

The overall look of the project will be the same as 5*6 - we’ll have a main class that creates the main window which handles setting up all of the parameters. We’ll keep the parameter changes for more detailed textures. It then creates a new realtime class, where we’ll initialize the VBOs& VAOs for the textile to render, and then paint it with calls to the VBO/VAO class. 

### Credits

Ria - I worked on getting the mass spring system set up and running, as well as put together the final presentation you saw today and recorded the video!
