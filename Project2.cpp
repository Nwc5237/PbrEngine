
#include <Project2.hpp>
#include <Windows.h>
#include <aclapi.h>
bool useTex = true, waitForRelease = false, stop_rotating = false;
glm::vec3 lightPos(0.0f);
float rot = 0.0f;


//mapping a value i from the range [a, b] to the range [x, y]
float map_val(float i, float a, float b, float x, float y)
{
	float percent_of_ab = (i - a) / (b - a);
	return percent_of_ab * (y - x) + x;
}

int main()
{
	// glfw: initialize and configure
	// ------------------------------

	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_SAMPLES, 4);

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // uncomment this statement to fix compilation on OS X
#endif

	// glfw window creation
	// --------------------
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Project 2", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	// tell GLFW to capture our mouse
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// glad: load all OpenGL function pointers
	// ---------------------------------------
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	// configure global opengl state
	// -----------------------------
	glEnable(GL_MULTISAMPLE); // Enabled by default on some drivers, but not all so always enable to make sure
	glEnable(GL_DEPTH_TEST);

	// build and compile shaders
	// -------------------------
	Shader skyboxShader("../Project_2/Shaders/skyboxShader.vert", "../Project_2/Shaders/skyboxShader.frag");
	Shader pbrShader("../Project_2/Shaders/pbrShader.vert", "../Project_2/Shaders/pbrShader.frag");

	// set up vertex data (and buffer(s)) and configure vertex attributes
	// ------------------------------------------------------------------
	float vertices[] = {
		// positions          // normals           // texture coords
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
		0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,
		0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
		0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,

		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
		0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
		0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
		0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,

		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
		-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
		-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

		0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
		0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
		0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
		0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
		0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
		0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
		0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  1.0f,
		0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
		0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,

		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
		0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
		0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
		0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f
	};

	// first, configure the cube's VAO (and VBO)
	unsigned int VBO, cubeVAO;
	glGenVertexArrays(1, &cubeVAO);
	glGenBuffers(1, &VBO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindVertexArray(cubeVAO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	// second, configure the light's VAO (VBO stays the same; the vertices are the same for the light object which is also a 3D cube)
	unsigned int lightVAO;
	glGenVertexArrays(1, &lightVAO);
	glBindVertexArray(lightVAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	// note that we update the lamp's position attribute's stride to reflect the updated buffer data
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// Skybox uses the same vertices so no need to make a new one.  Just making a new name for sanity
	unsigned int skyboxVAO = lightVAO;

	// load textures
	// -------------

	std::vector<std::string> faces =
	{
		"../Project_2/Media/skybox/right.jpg",
		"../Project_2/Media/skybox/left.jpg",
		"../Project_2/Media/skybox/top.jpg",
		"../Project_2/Media/skybox/bottom.jpg",
		"../Project_2/Media/skybox/back.jpg",
		"../Project_2/Media/skybox/front.jpg"
	};
	unsigned int cubemapTexture = loadCubemap(faces);

	//pbr texture loading
	unsigned int diffuse, roughness, metalness, normal;
	diffuse = loadTexture("../Project_2/Media/textures/metalCol.jpg");
	roughness = loadTexture("../Project_2/Media/textures/metalRoughness.jpg");
	metalness = loadTexture("../Project_2/Media/textures/metalMetalness.jpg");
	normal = loadTexture("../Project_2/Media/textures/metalNorm.jpg");


	// positions of the point lights
	glm::vec3 pointLightPositions[] = {
		glm::vec3(0.7f,  0.2f,  2.0f),
		glm::vec3(2.3f, -3.3f, -4.0f),
		glm::vec3(-4.0f,  2.0f, -12.0f),
		glm::vec3(0.0f,  0.0f, -3.0f)
	};

	// load models
	// -----------
	//Model ourModel("../Project_2/Media/nanosuit/nanosuit.obj");

	Model sphere("../Project_2/Media/SphereModel/Sphere.obj");
	//Model sphere("../Project_2/Media/cube_model/cube_model.fbx");


	bool first_time = true;
	// render loop
	// -----------
	while (!glfwWindowShouldClose(window))
	{
		// per-frame time logic
		// --------------------
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// weighted avg for framerate
		framerate = (0.4f / (deltaTime)+1.6f * framerate) / 2.0f;

		// input
		// -----
		processInput(window);

		// render
		// ------
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// draw scene as normal, get camera parameters
		glm::mat4 model;
		glm::mat4 view = camera.GetViewMatrix();
		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
		model = glm::rotate(model, glm::radians(10.0f*currentFrame), glm::vec3(1.0f, 0.3f, 0.5f));

		//MY PBR SHADER SETUP
		pbrShader.use();
		pbrShader.setMat4("model", model);
		pbrShader.setMat4("view", view);
		pbrShader.setMat4("projection", projection);
		pbrShader.setVec3("cameraPos", camera.Position);
		

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, diffuse);
		glUniform1i(glGetUniformLocation(pbrShader.ID, "diffuseTex"), 1);

		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, roughness);
		glUniform1i(glGetUniformLocation(pbrShader.ID, "roughnessTex"), 2);

		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, metalness);
		glUniform1i(glGetUniformLocation(pbrShader.ID, "metalnessTex"), 3);

		glActiveTexture(GL_TEXTURE4);
		glBindTexture(GL_TEXTURE_2D, normal);
		glUniform1i(glGetUniformLocation(pbrShader.ID, "normalTex"), 4);

		pbrShader.setBool("useTex", useTex);
		pbrShader.setVec3("lights[0].position", lightPos);
		pbrShader.setVec3("lights[0].color", glm::vec3(0.5f, 0.5f, 0.5f));
		pbrShader.setVec3("camPos", camera.Position);

		for (unsigned int i = 0; i < 16; i++)
		{
			// calculate the model matrix for each object and pass it to shader before drawing
			glm::mat4 box_model;

			box_model = glm::scale(box_model, glm::vec3(1.0f, 1.0f, 1.0f));
			box_model = glm::translate(box_model, glm::vec3((float)((int)i % (int)4) + 2  * (i%4) , (float)(i / 4) + 2 * (i/4), -5.0f));
			box_model = glm::rotate(box_model, rot, glm::vec3(0.0f, 1.0f, 0.0f));

			if(!stop_rotating)
				rot += .0005;

			pbrShader.setMat4("model", box_model);
			sphere.Draw(pbrShader);

		}

		first_time = false;
		glBindVertexArray(0);

		


		// draw skybox as last
		glDepthFunc(GL_LEQUAL);  // change depth function so depth test passes when values are equal to depth buffer's content
		skyboxShader.use();
		view = glm::mat4(glm::mat3(camera.GetViewMatrix())); // remove translation from the view matrix
		skyboxShader.setMat4("view", view);
		skyboxShader.setMat4("projection", projection);
		// skybox cube
		glBindVertexArray(skyboxVAO);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);
		
		glDepthFunc(GL_LESS); // set depth function back to default

							  // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
							  // -------------------------------------------------------------------------------
		
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// optional: de-allocate all resources once they've outlived their purpose:
	// ------------------------------------------------------------------------
	glDeleteVertexArrays(1, &cubeVAO);
	glDeleteVertexArrays(1, &skyboxVAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &skyboxVAO);

	glfwTerminate();
	return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
//   The movement of the boxes is still here.  Feel free to use it or take it out
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{

	// Escape Key quits
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	// Movement Keys  -  Need to disable this while you are moving along your track
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, deltaTime);

	// change stepsize multiplier (to make it less if necessary
	if (glfwGetKey(window, GLFW_KEY_COMMA))
		step_multiplier *= 1.01f;
	if (glfwGetKey(window, GLFW_KEY_PERIOD))
		step_multiplier /= 1.01f;

	// update step based on framerate (prevents excessive changes)
	float step = deltaTime * step_multiplier;


	// Changing overall behavior (only want these to trigger once so add a delay of half a second)
	float currentFrame = glfwGetTime();
	bool somethingPressed = glfwGetKey(window, GLFW_KEY_H) == GLFW_PRESS ||
		glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS ||
		glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS ||
		glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS ||
		glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS ||
		glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS ||
		glfwGetKey(window, GLFW_KEY_N) == GLFW_PRESS;
	if (somethingPressed && last_pressed < currentFrame - 0.5f || last_pressed == 0.0f)
	{
		if (glfwGetKey(window, GLFW_KEY_H) == GLFW_PRESS)
			drawHeightmap ? drawHeightmap = false : drawHeightmap = true;
		if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS)
			drawBoxes ? drawBoxes = false : drawBoxes = true;
		if (glfwGetKey(window, GLFW_KEY_N) == GLFW_PRESS)
			drawNormals ? drawNormals = false : drawNormals = true;
		if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
			if (quaterians)
			{
				quaterians = false;
				std::cout << "Not using Quaternions" << std::endl;
				rotation_euler = glm::eulerAngles(rotation);
			}
			else
			{
				quaterians = true;
				std::cout << "Using Quaternions" << std::endl;
				rotation = glm::quat(rotation_euler);
			}

		// reset all changes to 0
		if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS)
		{
			rotation_rate = glm::vec3(0.0f, 0.0f, 0.0f);
			scale = glm::vec3(1.0f, 1.0f, 1.0f);
			translation = glm::vec3(0.0f, 0.0f, 0.0f);
			rotation = glm::quat(glm::vec3(0.0f, 0.0f, 0.0f));
			rotation_euler = glm::vec3(0.0f, 0.0f, 0.0f);
			step_multiplier = 1.0f;
		}

		if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
		{
			rotation_rate = 50.0f*glm::vec3(M_PI / 64.0f, M_PI / 64.0f, M_PI / 64.0f);
			scale = glm::vec3(2.0f, 0.5f, 0.2f);
			translation = glm::vec3(0.0f, 0.0f, 0.0f);
			rotation = glm::quat(glm::vec3(0.0f, 0.0f, 0.0f));
			rotation_euler = glm::vec3(0.0f, 0.0f, 0.0f);
		}

		// Print all info
		if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS)
		{
			std::printf("Frame Rate: %.05f\nCurrent Frame: %.05f\tLast Pressed: %.05f\n", framerate, currentFrame, last_pressed);
			std::printf("Step: %.05f\tStep Multiplier: %.04f\n", step, step_multiplier);
			std::printf("Rotation Rate (%.05f,%.05f,%.05f)\n", rotation_rate.x, rotation_rate.y, rotation_rate.z);
			std::printf("Rotation Euler (%.05f,%.05f,%.05f)\n", rotation_euler.x, rotation_euler.y, rotation_euler.z);
			std::printf("Rotation Quaterians (%.05f,%.05f,%.05f,%.05f)\n", rotation.x, rotation.y, rotation.z, rotation.w);
			std::printf("Translation (%.05f,%.05f,%.05f)\n", rotation_rate.x, rotation_rate.y, rotation_rate.z);
			std::printf("Scale (%.05f,%.05f,%.05f)\n", scale.x, scale.y, scale.z);
			std::printf("Front (%.05f,%.05f,%.05f)\n", camera.Front.x, camera.Front.y, camera.Front.z);
			quaterians ? std::printf("Using Quaterians\n") : std::printf("Not Using Quaterians\n");
			std::printf("\n");

			
		}


		last_pressed = currentFrame;
	}


	if (glfwGetKey(window, GLFW_KEY_COMMA) || glfwGetKey(window, GLFW_KEY_PERIOD))
		std::printf("Step: %.05f\tStep Multiplier: %.04f\\tFrame Rate: %.05f\n", step, step_multiplier, framerate);

	if (glfwGetKey(window, GLFW_KEY_U) == GLFW_PRESS)
		lightPos.x += step;
	if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS)
		lightPos.y += step;
	if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS)
		lightPos.z += step;
	if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS)
		lightPos.x -= step;
	if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS)
		lightPos.y -= step;
	if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS)
		lightPos.z -= step;


	//doing roughness change in shader
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
		waitForRelease = true;

	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_RELEASE && waitForRelease)
	{
		useTex = !useTex;
		//stop_rotating = !stop_rotating;
		waitForRelease = false;
	}

	// figure out what to change
	bool shift = glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) || glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT);
	bool ctrl = glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) || glfwGetKey(window, GLFW_KEY_RIGHT_CONTROL);




}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
	SCR_WIDTH = width;
	SCR_HEIGHT = height;

}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(yoffset);
}

// utility function for loading a 2D texture from file
// ---------------------------------------------------
unsigned int loadTexture(char const * path)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);

	int width, height, nrComponents;
	unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
	if (data)
	{
		GLenum format;
		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}
	else
	{
		std::cout << "Texture failed to load at path: " << path << std::endl;
		stbi_image_free(data);
	}

	return textureID;
}

// loads a cubemap texture from 6 individual texture faces
// order:
// +X (right)
// -X (left)
// +Y (top)
// -Y (bottom)
// +Z (front) 
// -Z (back)
// -------------------------------------------------------
unsigned int loadCubemap(std::vector<std::string> faces)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

	int width, height, nrComponents;
	for (unsigned int i = 0; i < faces.size(); i++)
	{
		unsigned char *data = stbi_load(faces[i].c_str(), &width, &height, &nrComponents, 0);
		if (data)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			stbi_image_free(data);
		}
		else
		{
			std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
			stbi_image_free(data);
		}
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	return textureID;
}

void set_lighting(Shader shader, glm::vec3 * pointLightPositions)
{
	shader.use();
	shader.setVec3("viewPos", camera.Position);

	/*
	Here we set all the uniforms for the 5/6 types of lights we have. We have to set them manually and index
	the proper PointLight struct in the array to set each uniform variable. This can be done more code-friendly
	by defining light types as classes and set their values in there, or by using a more efficient uniform approach
	by using 'Uniform buffer objects', but that is something we'll discuss in the 'Advanced GLSL' tutorial.
	*/
	// directional light
	//shader.setVec3("dirLight.direction", -0.2f, -1.0f, -0.3f);
	shader.setVec3("dirLight.direction", 0.24f, -.3f, 0.91f); // Tried to target the sun
	shader.setVec3("dirLight.ambient", 0.05f, 0.05f, 0.05f);
	shader.setVec3("dirLight.diffuse", 0.5f, 0.5f, 0.5f);
	shader.setVec3("dirLight.specular", 0.5f, 0.5f, 0.5f);
	// point light 1
	shader.setVec3("pointLights[0].position", pointLightPositions[0]);
	shader.setVec3("pointLights[0].ambient", 0.05f, 0.05f, 0.05f);
	shader.setVec3("pointLights[0].diffuse", 0.8f, 0.8f, 0.8f);
	shader.setVec3("pointLights[0].specular", 1.0f, 1.0f, 1.0f);
	shader.setFloat("pointLights[0].constant", 1.0f);
	shader.setFloat("pointLights[0].linear", 0.09);
	shader.setFloat("pointLights[0].quadratic", 0.032);
	// point light 2
	shader.setVec3("pointLights[1].position", pointLightPositions[1]);
	shader.setVec3("pointLights[1].ambient", 0.05f, 0.05f, 0.05f);
	shader.setVec3("pointLights[1].diffuse", 0.8f, 0.8f, 0.8f);
	shader.setVec3("pointLights[1].specular", 1.0f, 1.0f, 1.0f);
	shader.setFloat("pointLights[1].constant", 1.0f);
	shader.setFloat("pointLights[1].linear", 0.09);
	shader.setFloat("pointLights[1].quadratic", 0.032);
	// point light 3
	shader.setVec3("pointLights[2].position", pointLightPositions[2]);
	shader.setVec3("pointLights[2].ambient", 0.05f, 0.05f, 0.05f);
	shader.setVec3("pointLights[2].diffuse", 0.8f, 0.8f, 0.8f);
	shader.setVec3("pointLights[2].specular", 1.0f, 1.0f, 1.0f);
	shader.setFloat("pointLights[2].constant", 1.0f);
	shader.setFloat("pointLights[2].linear", 0.09);
	shader.setFloat("pointLights[2].quadratic", 0.032);
	// point light 4
	shader.setVec3("pointLights[3].position", pointLightPositions[3]);
	shader.setVec3("pointLights[3].ambient", 0.05f, 0.05f, 0.05f);
	shader.setVec3("pointLights[3].diffuse", 0.8f, 0.8f, 0.8f);
	shader.setVec3("pointLights[3].specular", 1.0f, 1.0f, 1.0f);
	shader.setFloat("pointLights[3].constant", 1.0f);
	shader.setFloat("pointLights[3].linear", 0.09);
	shader.setFloat("pointLights[3].quadratic", 0.032);
	// spotLight
	shader.setVec3("spotLight.position", camera.Position);
	shader.setVec3("spotLight.direction", camera.Front);
	shader.setVec3("spotLight.ambient", 0.0f, 0.0f, 0.0f);
	shader.setVec3("spotLight.diffuse", 1.0f, 1.0f, 1.0f);
	shader.setVec3("spotLight.specular", 1.0f, 1.0f, 1.0f);
	shader.setFloat("spotLight.constant", 1.0f);
	shader.setFloat("spotLight.linear", 0.09);
	shader.setFloat("spotLight.quadratic", 0.032);
	shader.setFloat("spotLight.cutOff", glm::cos(glm::radians(12.5f)));
	shader.setFloat("spotLight.outerCutOff", glm::cos(glm::radians(15.0f)));

}
