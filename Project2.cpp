
#include <Project2.hpp>
#include <Windows.h>
#include <aclapi.h>
#include <fileapi.h>
#include <ddraw.h>
#include "EngineUtils.hpp"
#include "JsonParser.hpp"


#define GLM_ENABLE_EXPERIMENTAL
//#define USE_SCENE_FILE //comment this out when using single objects

bool useTex = true, waitForRelease = false,
usePBR = true, waitForRelease2 = false;
glm::vec3 lightPos(0.0f, 5.0f, 0.0f);

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
	glfwSetCursorPosCallback(window, mouse_move_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);
	glfwSetScrollCallback(window, scroll_callback);

	// tell GLFW to capture our mouse
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

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
	glEnable(GL_STENCIL_TEST);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

#ifdef USE_SCENE_FILE
	/*Right here we're just reading in the json file*/
	std::string json;
	std::fstream newfile;
	newfile.open("D:\\OpenGL_EdenExport\\scene_file.txt", std::ios::in);
	if (newfile.is_open()) {
		std::string line;
		getline(newfile, line);
		json.append(line);
		newfile.close();
	}


	//now we're parsing the json file
	JsonParser jsonParser;
	jsonParser.lexFile(json);
	std::vector<JsonSceneComponent> components = jsonParser.parseJson();

	std::vector<ObjModel> sceneModels;
	std::string modelFolder("D:\\OpenGL_EdenExport\\assets\\");
	for (int i = 0; i < components.size(); i++) {
		modelFolder = std::string("D:\\OpenGL_EdenExport\\assets\\");
		modelFolder.append(components.at(i).modelPath.c_str());
		sceneModels.push_back(ObjModel(modelFolder.c_str()));
		printf("loaded %d/%d models\n", i, components.size());
	}
#endif
	ObjModel obj_model = ObjModel("C:/Users/ncala/Downloads/PBR_gun.obj");

	for(int i = 1; i < 6; i++)
	{
		gameObjects.push_back(GameObject(obj_model, glm::vec3(0.0f, -i, 0.0f)));
		gameObjects.back().stencilColor = glm::vec3(0.0f, 0.0f, i / 255.0f);
	}

	// build and compile shaders
	// -------------------------
	Shader pbrShader("../Project_2/Shaders/pbrShader.vert", "../Project_2/Shaders/pbrShader.frag");
	Shader defShader("../Project_2/Shaders/QuadShader.vert", "../Project_2/Shaders/QuadShader.frag");

	//Multiple render targets for deferred rendering
	unsigned int gBuffer, gPosition, gNormal, gAlbedo, gRoughMetal, gAmbient; //metalness and roughness can be in the alpha channels of position and normal since they're one number
	glGenFramebuffers(1, &gBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);

	glGenTextures(1, &gPosition);
	glBindTexture(GL_TEXTURE_2D, gPosition);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gPosition, 0);

	glGenTextures(1, &gNormal);
	glBindTexture(GL_TEXTURE_2D, gNormal);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gNormal, 0);

	glGenTextures(1, &gAlbedo);
	glBindTexture(GL_TEXTURE_2D, gAlbedo);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gAlbedo, 0);

	glGenTextures(1, &gRoughMetal);
	glBindTexture(GL_TEXTURE_2D, gRoughMetal);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, gRoughMetal, 0);

	glGenTextures(1, &gAmbient);
	glBindTexture(GL_TEXTURE_2D, gAmbient);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT4, GL_TEXTURE_2D, gAmbient, 0);

	unsigned int attachments[5] = { GL_COLOR_ATTACHMENT0 , GL_COLOR_ATTACHMENT1 , GL_COLOR_ATTACHMENT2 , GL_COLOR_ATTACHMENT3, GL_COLOR_ATTACHMENT4 };

	glDrawBuffers(5, attachments);

	unsigned int rboDepth;
	glGenRenderbuffers(1, &rboDepth);
	glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, SCR_WIDTH, SCR_HEIGHT);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);
	
	// finally check if framebuffer is complete
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "Framebuffer not complete!" << std::endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		return false;

	//return frame buffer
	glBindFramebuffer(GL_FRAMEBUFFER, 0);



	// render loop
	// -----------

	float percent = 0.f;
	while (!glfwWindowShouldClose(window))
	{
		// per-frame time logic
		// --------------------
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// weighted avg for framerate
		framerate = (0.4f / (deltaTime)+1.6f * framerate) / 2.0f;
		printf("framerate: %f\n", framerate);

		// input
		// -----
		processInput(window);

		// render
		// ------
		
		//new stuf I added
		glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
		glEnable(GL_DEPTH_TEST);

		view = camera.GetViewMatrix();
		projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
		model = glm::rotate(model, glm::radians(10.0f * currentFrame), glm::vec3(1.0f, 0.3f, 0.5f));

		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

		//MY PBR SHADER SETUP
		pbrShader.use();
		pbrShader.setMat4("model", model);
		pbrShader.setMat4("view", view);
		pbrShader.setMat4("projection", projection);
		pbrShader.setVec3("camPos", camera.Position);

		//testing uniform bools
		pbrShader.setBool("useTex", useTex);
		pbrShader.setBool("usePBR", usePBR);

		pbrShader.setVec3("lights[0].position", lightPos);
		pbrShader.setVec3("lights[0].color", glm::vec3(0.5f, 0.5f, 0.5f));
		pbrShader.setVec3("lights[1].position", glm::vec3(-5.000000, -2.400000, -27.600033));
		pbrShader.setVec3("lights[1].color", glm::vec3(0.5f, 0.5f, 0.5f));
		pbrShader.setVec3("lights[2].position", glm::vec3(-14.799991, -3.200000, -27.800034));
		pbrShader.setVec3("lights[2].color", glm::vec3(0.5f, 0.5f, 0.5f));
		pbrShader.setVec3("lights[3].position", glm::vec3(-5.200000, -2.200000, -19.200001));
		pbrShader.setVec3("lights[3].color", glm::vec3(0.5f, 0.5f, 0.5f));

		for (GameObject gameObject : gameObjects)
		{
			gameObject.draw(pbrShader);
		}

		#ifdef USE_SCENE_FILE
		for (int i = 0; i < sceneModels.size(); i++) {

			//for every instance of this object
			for (int j = 0; j < components.at(i).transformations.size(); j++) {
				model = glm::mat4(1.0f);

				glm::vec3 temp = components.at(i).transformations.at(j).location;
				glm::vec3 trans(temp.x, temp.z, -(temp.y - 300.f));
				model = glm::translate(model, percent * trans);
				//model = glm::translate(model, components.at(i).transformations.at(j).location);

				//all of these axes are weird because of blender's axes. Find a cleaner way probably
				model = glm::rotate(model, glm::radians(components.at(i).transformations.at(j).rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
				model = glm::rotate(model, glm::radians(components.at(i).transformations.at(j).rotation.y), glm::vec3(0.0f, 0.0f, -1.0f));
				model = glm::rotate(model, glm::radians(components.at(i).transformations.at(j).rotation.z), glm::vec3(0.0f, 1.0f, 0.0f));

				temp = components.at(i).transformations.at(j).scale;
				model = glm::scale(model, glm::vec3(temp.x, temp.z, temp.y));

				pbrShader.setMat4("model", model);
				sceneModels.at(i).Draw(pbrShader);
			}
		}

		//that thing where the scene builds itself
		if (percent < 1)
			percent += .001;
		#endif

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glDisable(GL_DEPTH_TEST);


		//start
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		defShader.use();
		defShader.setBool("useTex", useTex);
		
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, gPosition);
		glUniform1i(glGetUniformLocation(defShader.ID, "PositionRoughness"), 0);

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, gNormal);
		glUniform1i(glGetUniformLocation(defShader.ID, "NormalMetalness"), 1);

		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, gAlbedo);
		glUniform1i(glGetUniformLocation(defShader.ID, "Albedo"), 2);

		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, gRoughMetal);
		glUniform1i(glGetUniformLocation(defShader.ID, "RoughMetal"), 3);

		glActiveTexture(GL_TEXTURE4);
		glBindTexture(GL_TEXTURE_2D, gAmbient);
		glUniform1i(glGetUniformLocation(defShader.ID, "Ambient"), 4);
		
		defShader.setFloat("fader", fader);
		defShader.setVec3("camPos", camera.Position);
		renderQuad();

		glBindVertexArray(0);
		glDepthFunc(GL_LESS);
		glfwSwapBuffers(window);

		pbrShader.use();
		//now we do selection using one of the components of the albedo texture ---------------------------------------------------
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//this needs to happen *before* polling events because we don't want to clear the albedo color
		glDisable(GL_MULTISAMPLE);
		pbrShader.setBool("isSelectionBuffer", true);
		for (int i = 0; i < gameObjects.size(); i++) //would use for each, but need index
		{
			GameObject gameObject = gameObjects.at(i);
			pbrShader.setVec3("selectionIndex", gameObject.stencilColor); //+1 required since 0 is for nothing
			gameObject.draw(pbrShader); //wait why isn't this overwriting the albedo?
		}
		pbrShader.setBool("isSelectionBuffer", false);
		glEnable(GL_MULTISAMPLE);

		glfwPollEvents(); //polling events after the albedo texture functions as a stencil buffer
	}

	// optional: de-allocate all resources once they've outlived their purpose:
	// ------------------------------------------------------------------------
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
		fader -= .5;
	if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS)
		fader += .5;

	//doing roughness change in shader
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
		waitForRelease = true;

	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_RELEASE && waitForRelease)
	{
		useTex = !useTex;
		//stop_rotating = !stop_rotating;
		waitForRelease = false;
	}

	//doing roughness change in shader
	if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
		waitForRelease2 = true;

	if (glfwGetKey(window, GLFW_KEY_R) == GLFW_RELEASE && waitForRelease2)
	{
		usePBR = !usePBR;
		//stop_rotating = !stop_rotating;
		waitForRelease2 = false;
	}

	if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS)
		fader -= .5;
	if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS)
		fader += .5;

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
void mouse_move_callback(GLFWwindow* window, double xpos, double ypos)
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

	if (isPanning)
	{
		camera.ProcessMouseMovement(xoffset, yoffset);
	}

	cursor_pos_x = floor(xpos);
	cursor_pos_y = floor(ypos);
}

//going to use these to do an unProject, then "select" the object with the nearest (x, y) coords
//Will probably want to use a quad tree. Maybe not yet though since click actions won't be super
//common, and even if it takes a bit, it's not imporant that it's fast just yet. Just want it working
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
	{
		int pixel_blockx, pixel_blocky;
		glm::vec4 color;
		
		pixel_blockx = 1;
		pixel_blocky = 1;
		
		glReadPixels(cursor_pos_x,
			SCR_HEIGHT - cursor_pos_y - 1,
			pixel_blockx,
			pixel_blocky,
			GL_RGBA,
			GL_FLOAT,
			&color);

		printf("this is the color: (%f, %f, %f)\n", color.x, color.y, color.z);

		for(GameObject & gameObject : gameObjects)
		{
			//might want a float equals ---  but that also might be a waste of time, since no calculations are done on the float (that I 'm aware of) so they shouldn't change
			if (equals(gameObject.stencilColor.x, color.x)
				&& equals(gameObject.stencilColor.y, color.y)
				&& equals(gameObject.stencilColor.z, color.z)
				)
			{
				printf("that's a hit!\n");
				gameObject.isSelected = !gameObject.isSelected;
			}
		}
	}
	else if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
	{
		//start panning
		isPanning = true;
	}
	else if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE)
	{
		//end panning
		isPanning = false;
	}
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

void renderQuad()
{
	if (quadVAO == 0)
	{
		float quadVertices[] = {
			// positions        // texture Coords
			-1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
			-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
			 1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
			 1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
		};
		// setup plane VAO
		glGenVertexArrays(1, &quadVAO);
		glGenBuffers(1, &quadVBO);
		glBindVertexArray(quadVAO);
		glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	}
	glBindVertexArray(quadVAO);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glBindVertexArray(0);
}
