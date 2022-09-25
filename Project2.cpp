
#include <Project2.hpp>
#include <Windows.h>
#include <aclapi.h>
#include <fileapi.h>
#include <ddraw.h>
#include "EngineUtils.hpp"
#include "JsonParser.hpp"


#define GLM_ENABLE_EXPERIMENTAL
//#include <D:\PA2_Starter\Vendor\gli-0.8.2.0\gli\gli\gli.hpp>

bool useTex = true, waitForRelease = false, stop_rotating = false;
glm::vec3 lightPos(0.0f);
float rot = 0.0f;
float fader = 50.0f;

//mapping a value i from the range [a, b] to the range [x, y]
float map_val(float i, float a, float b, float x, float y)
{
	float percent_of_ab = (i - a) / (b - a);
	return percent_of_ab * (y - x) + x;
}

vector<Model> load_scene(const char* folder)
{
	vector<Model> models;

	return models;

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
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	std::string json("{\"ColumbiaBldgSet_Tunnel\": [{\"location\": [4.42001485824585, 363.9205322265625, -58.55177688598633], \"rotation\": [0.0, -0.0, 0.0], \"scale\": [1.9501981735229492, 1.9501981735229492, 1.9501981735229492]}]}");
	JsonParser p;
	p.lexFile(json);
	std::vector<JsonSceneComponent> components = p.parseJson();

	ObjModel obj_model = ObjModel("C:/Users/ncala/Downloads/test_obj_file.obj");
	//ObjModel obj_model = ObjModel("C:/Users/ncala/Downloads/test_two_mesh_object.obj");
	//ObjModel obj_model = ObjModel("C:/Users/ncala/Downloads/enterprise.obj");


	// build and compile shaders
	// -------------------------
	Shader skyboxShader("../Project_2/Shaders/skyboxShader.vert", "../Project_2/Shaders/skyboxShader.frag");
	Shader pbrShader("../Project_2/Shaders/pbrShader.vert", "../Project_2/Shaders/pbrShader.frag");
	Shader hdriMapShader("../Project_2/Shaders/Cubemap.vert", "../Project_2/Shaders/Cubemap.frag");

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

	// load models
	// -----------
	Model sphere("../Project_2/Media/SphereModel/Sphere.obj");
	//Model hall("../Project_2/Media/PT_assets/PTHallway2.obj");

	//Model sphere("../Project_2/Media/cube_model/cube_model.fbx");
	//load_scene("D:\BioshockClone\BlenderScene\NiceExport2");
	Model hall("C:/Users/ncala/Downloads/Cerberus_by_Andrew_Maximov/Cerberus_LP.fbx");

	//hdr map load
	unsigned hdr_tex_id = load_environment_map("../Project_2/Media/textures/noon_grass_1k.hdr");

	//pbr texture loading
	unsigned int diffuse, roughness, metalness, normal;
	/*	diffuse = loadTexture("../Project_2/Media/textures/metalCol.jpg");
		roughness = loadTexture("../Project_2/Media/textures/metalRoughness.jpg");
		metalness = loadTexture("../Project_2/Media/textures/metalMetalness.jpg");
		normal = loadTexture("../Project_2/Media/textures/metalNorm.jpg");*/

	diffuse = loadTexture("C:/Users/ncala/Downloads/Cerberus_A.jpg");
	roughness = loadTexture("C:/Users/ncala/Downloads/Cerberus_R.jpg");
	metalness = loadTexture("C:/Users/ncala/Downloads/Cerberus_M.jpg");
	normal = loadTexture("C:/Users/ncala/Downloads/Cerberus_N.jpg");

	diffuse = loadTexture("C:/Users/ncala/Downloads/Cerberus_by_Andrew_Maximov/Textures/Cerberus_A.tga");
	roughness = loadTexture("C:/Users/ncala/Downloads/Cerberus_by_Andrew_Maximov/Textures/Cerberus_R.tga");
	metalness = loadTexture("C:/Users/ncala/Downloads/Cerberus_by_Andrew_Maximov/Textures/Cerberus_M.tga");
	normal = loadTexture("C:/Users/ncala/Downloads/Cerberus_by_Andrew_Maximov/Textures/Cerberus_N.tga");

	int* bufsize, * nummips;
	//GLuint img = texture_loadDDS("D:/PT_Remake_Blender/textures/shsb_hous001_w1_nrm.dds");

	// positions of the point lights
	glm::vec3 pointLightPositions[] = {
		glm::vec3(0.7f,  0.2f,  2.0f),
		glm::vec3(2.3f, -3.3f, -4.0f),
		glm::vec3(-4.0f,  2.0f, -12.0f),
		glm::vec3(0.0f,  0.0f, -3.0f)
	};

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
		model = glm::rotate(model, glm::radians(10.0f * currentFrame), glm::vec3(1.0f, 0.3f, 0.5f));

		//MY PBR SHADER SETUP
		pbrShader.use();
		pbrShader.setMat4("model", model);
		pbrShader.setMat4("view", view);
		pbrShader.setMat4("projection", projection);
		pbrShader.setVec3("cameraPos", camera.Position);

		pbrShader.setBool("useTex", useTex);
		pbrShader.setVec3("lights[0].position", lightPos);
		pbrShader.setVec3("lights[0].color", glm::vec3(0.5f, 0.5f, 0.5f));
		pbrShader.setVec3("camPos", camera.Position);
		pbrShader.setFloat("fader", fader);

		pbrShader.setVec3("lights[1].position", glm::vec3(-5.000000, -2.400000, -27.600033));
		pbrShader.setVec3("lights[1].color", glm::vec3(0.5f, 0.5f, 0.5f));
		pbrShader.setVec3("lights[2].position", glm::vec3(-14.799991, -3.200000, -27.800034));
		pbrShader.setVec3("lights[2].color", glm::vec3(0.5f, 0.5f, 0.5f));
		pbrShader.setVec3("lights[3].position", glm::vec3(-5.200000, -2.200000, -19.200001));
		pbrShader.setVec3("lights[3].color", glm::vec3(0.5f, 0.5f, 0.5f));


		/**/glActiveTexture(GL_TEXTURE1);
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
		glUniform1i(glGetUniformLocation(pbrShader.ID, "normalTex"), 4);/**/

		model = glm::mat4(1.0f);

		model = glm::translate(model, lightPos);
		model = glm::scale(model, glm::vec3(.1f));

		pbrShader.setMat4("model", model);
		//sphere.Draw(pbrShader); //looks like passing this value isn't adding anything currently

		obj_model.Draw(pbrShader);

		for (unsigned int i = 0; i < 1; i++)
		{
			// calculate the model matrix for each object and pass it to shader before drawing
			glm::mat4 box_model;

			box_model = glm::scale(box_model, glm::vec3(.01f, .01f, .01f));

			box_model = glm::translate(box_model, glm::vec3((float)((int)i % (int)5) + 2 * (i % 5), (float)(i / 5) + 2 * (i / 5), -5.0f));
			box_model = glm::translate(box_model, glm::vec3(-5, -5, 0));
			//box_model = glm::rotate(box_model, rot, glm::vec3(0.0f, 1.0f, 0.0f));
			box_model = glm::rotate(box_model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));

			pbrShader.setFloat("roughnessF", map_val(i, 0, 25, 0, 1));
			pbrShader.setFloat("metalnessF", map_val(i, 0, 25, 0, 1));

			if (!stop_rotating)
				rot += .00005;

			pbrShader.setMat4("model", box_model);
			//hall.Draw(pbrShader);

			box_model = glm::scale(box_model, glm::vec3(1, 1, 1));
			pbrShader.setMat4("model", box_model);
			//sphere.Draw(pbrShader);

		}
		glBindVertexArray(0);




		// draw skybox as last
		glDepthFunc(GL_LEQUAL);  // change depth function so depth test passes when values are equal to depth buffer's content
		hdriMapShader.use();

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, hdr_tex_id);
		glUniform1i(glGetUniformLocation(hdriMapShader.ID, "equirectangularMap"), 1);

		//view = glm::mat4(glm::mat3(camera.GetViewMatrix())); // remove translation from the view matrix
		hdriMapShader.setMat4("view", view);
		hdriMapShader.setMat4("projection", projection);
		// skybox cube
		glBindVertexArray(skyboxVAO);
		glActiveTexture(GL_TEXTURE0);
		//glDrawArrays(GL_TRIANGLES, 0, 36);
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
void processInput(GLFWwindow* window)
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


	if (glfwGetKey(window, GLFW_KEY_COMMA) || glfwGetKey(window, GLFW_KEY_PERIOD))
		std::printf("Step: %.05f\tStep Multiplier: %.04f\\tFrame Rate: %.05f\n", step, step_multiplier, framerate);

	float step1 = .02f;
	if (glfwGetKey(window, GLFW_KEY_U) == GLFW_PRESS)
		lightPos.x += step1;
	if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS)
		lightPos.y += step1;
	if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS)
		lightPos.z += step1;
	if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS)
		lightPos.x -= step1;
	if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS)
		lightPos.y -= step1;
	if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS)
		lightPos.z -= step1;

	if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS)
		fader -= .1;
	if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS)
		fader += .1;

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
unsigned int loadTexture(char const* path)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);

	int width, height, nrComponents;
	unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);
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
		std::cout << "Texture fai4led to load at path: " << path << std::endl;
		stbi_image_free(data);
	}

	return textureID;
}

unsigned load_environment_map(const char* path)
{
	stbi_set_flip_vertically_on_load(true);
	int width, height, nr_components;
	unsigned hdr_texture_id;

	//load the hdri map
	float* hdri_raw = stbi_loadf(path, &width, &height, &nr_components, 0);

	//if loading didn't fail, create a texture for it
	if (hdri_raw)
	{
		glGenTextures(1, &hdr_texture_id);
		glBindTexture(GL_TEXTURE_2D, hdr_texture_id);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, hdri_raw);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		//be a good citizen
		stbi_image_free(hdri_raw);
	}
	else
	{
		cout << "HDRI map failed to load at path: " << path << endl;
	}

	//return the texture id
	return hdr_texture_id;
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
		unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrComponents, 0);
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