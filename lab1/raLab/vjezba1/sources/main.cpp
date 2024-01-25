// Local Headers
#include "Shader.h"
#include "FPSManager.h"

// System Headers
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp> // after <glm/glm.hpp>


#include <assimp/Importer.hpp>      
#include <assimp/scene.h>           
#include <assimp/postprocess.h> 

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

// Standard Headers
#include <cstdio>
#include <cstdlib>
#include <vector> 
#include <iostream>
#include <math.h>

#define DIVISIONS 30
#define FPS 60

bool crtajKontrolniPoligon = true;
bool crtajKrivulju = true;
bool crtajTangente = true;
bool crtajNormale = true;

// ucitava vertex shader i fragment shader istog imena
Shader* loadShader(char* path, char* naziv);
void framebuffer_size_callback(GLFWwindow* window, int Width, int Height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void key_callback(GLFWwindow* window, int key, int scanxode, int action, int mods);





// =========== KLASA TRANSFORM ================
class Transform {
public:
	glm::mat4 modelMatrix;
	glm::mat4 viewMatrix;
	glm::mat4 perspectiveMatrix;

	Transform()
	{
		modelMatrix = glm::mat4(1.0f);
		viewMatrix = glm::mat4(1.0f);
		perspectiveMatrix = glm::mat4(1.0f);
	}

	// https://stackoverflow.com/questions/8844585/glm-rotate-usage-in-opengl
	// vraca rotacijsku matricu
	glm::mat4 Rotation(char axis, float angle)
	{
		glm::mat4 rotationMatrix = glm::mat4(1.0f);
		glm::vec3 axisVector;

		if (axis == 'x') axisVector = glm::vec3(1.0f, 0.0f, 0.0f);
		else if (axis == 'y') axisVector = glm::vec3(0.0f, 1.0f, 0.0f);
		else if (axis == 'z') axisVector = glm::vec3(0.0f, 0.0f, 1.0f);
		else axisVector = glm::vec3(0.0f, 0.0f, 0.0f);

		rotationMatrix = glm::rotate(rotationMatrix, glm::radians(angle), axisVector);
		
		return rotationMatrix;
	}

	// vraca translacijsku matricu
	glm::mat4 Translation(glm::vec3 translationVector)
	{
		glm::mat4 translationMatrix = glm::mat4(1.0f);
		translationMatrix = glm::translate(translationMatrix, translationVector);
		return translationMatrix;
	}

	// vraca skaliracijsku(?) matricu
	glm::mat4 Scaling(glm::vec3 scalingVector)
	{
		glm::mat4 scalingMatrix = glm::mat4(1.0f);
		scalingMatrix = glm::scale(scalingMatrix, scalingVector);
		return scalingMatrix;
	}
};








// =========== KLASA MESH ================
struct Vertex {
	glm::vec3 Position;
};
class Mesh {
private:
	// postavljanje buffera
	void SetupMesh()
	{
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		glGenBuffers(1, &EBO);

		glBindVertexArray(VAO);

		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
		glEnableVertexAttribArray(0);
		
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), (void*)&indices[0], GL_STATIC_DRAW);
		
		glBindVertexArray(0);
	}
public:
	GLuint VAO;
	GLuint VBO; 
	GLuint EBO;
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	Transform transformObject;
	glm::mat4 normalMatrix;
	float xmax, xmin, ymax, ymin, zmax, zmin, xmid, ymid, zmid, M;

	// Constructor
	Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices)
	{
		this->vertices = vertices;
		this->indices = indices;
		
		transformObject = Transform();
		normalMatrix = glm::mat4(1.0f);

		SetupMesh();
	}

	// crtaj Mesh pomocu shadera
	void Draw(Shader* shader)
	{
		glUseProgram(shader->ID); // koristi zadani shader
		glBindVertexArray(VAO); // koristi VAO mesha

		// slanje uniformnih matrica u shader
		shader->setUniformMat4("perspective", transformObject.perspectiveMatrix);
		shader->setUniformMat4("view", transformObject.viewMatrix);
		shader->setUniformMat4("model", transformObject.modelMatrix);

		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // iscrtavanje samo obruba

		glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, (const void*)0); // crtaj
		glBindVertexArray(0);
	}

	// crtaj Mesh pomocu shadera i modelMatrixa
	void DrawMoreMeshes(Shader* shader, glm::mat4 modelMatrix)
	{
		glUseProgram(shader->ID); // koristi zadani shader
		glBindVertexArray(VAO); // koristi VAO mesha

		// slanje uniformnih matrica u shader
		shader->setUniformMat4("perspective", transformObject.perspectiveMatrix);
		shader->setUniformMat4("view", transformObject.viewMatrix);
		shader->setUniformMat4("model", modelMatrix);

		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // iscrtavanje samo obruba

		glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, (const void*)0);
		glBindVertexArray(0);
	}

	// updateaj normalMatrix
	void NormalMatrixUpdate(glm::mat4 transformationMatrix)
	{
		normalMatrix = transformationMatrix * normalMatrix;
	}

	// sve vertexe normaliziraj (pomocu normalMatrix) i reloadaj buffer
	void Normalize()
	{
		std::vector<Vertex> newVertices;

		for (unsigned int i = 0; i < vertices.size(); i++)
		{
			glm::vec4 newVector = normalMatrix * glm::vec4(vertices[i].Position, 1.0f);
			Vertex newVertex;
			newVertex.Position.x = newVector.x;
			newVertex.Position.y = newVector.y;
			newVertex.Position.z = newVector.z;
			newVertices.push_back(newVertex);
		}

		// zamjena starih podataka na bufferu novima
		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, newVertices.size() * sizeof(Vertex), &newVertices[0], GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
		glEnableVertexAttribArray(0);
		glBindVertexArray(0);
	}
};















// =========== KLASA OBJECT ================
class Object {

public:
	std::vector<Mesh> meshes;

	// konstruktor
	Object(std::string path)
	{
		LoadModel(path);	
	}

	// crtaj pomocu shadera
	void Draw(Shader* shader)
	{
		for (unsigned int i = 0; i < meshes.size(); i++)
		{
			meshes[i].Draw(shader);
		}
	}

	// crtaj više modela, svaki sa svojom model matricom
	void DrawMoreModels(Shader* shader, int numModels, std::vector<glm::mat4> modelMatrices)
	{
		for (unsigned int i = 0; i < numModels; i++)
		{
			for (unsigned int j = 0; j < meshes.size(); j++)
			{
				meshes[j].DrawMoreMeshes(shader, modelMatrices[i]);
			}
		}
	}

	// updateaj normalMatrix svakog mesha novom transformacijom
	void UpdateMeshNormalMatrices(glm::mat4 transformationMatrix)
	{
		for (unsigned int i = 0; i < meshes.size(); i++)
		{
			meshes[i].NormalMatrixUpdate(transformationMatrix);
		}
	}

	// updateaj perspectiveMatrix Transform objekta u svakom meshu
	void UpdateMeshPerspectiveMatrices(glm::mat4 perspectiveMatrix)
	{
		for (unsigned int i = 0; i < meshes.size(); i++)
		{
			meshes[i].transformObject.perspectiveMatrix = perspectiveMatrix;
		}
	}

	// updateaj viewMatrix Transform objekta u svakom meshu
	void UpdateMeshViewMatrices(glm::mat4 viewMatrix)
	{
		for (unsigned int i = 0; i < meshes.size(); i++)
		{
			meshes[i].transformObject.viewMatrix = viewMatrix;
		}
	}

	// updateaj modelMatrix svakog mesha zadanim modelMatrixom
	void UpdateMeshModelMatrices(glm::mat4 modelMatrix)
	{
		for (unsigned int i = 0; i < meshes.size(); i++)
		{
			meshes[i].transformObject.modelMatrix = modelMatrix * meshes[i].transformObject.modelMatrix;
		}
	}

	// zamijeni modelMatrix svakog mesha zadanim modelMatrixom
	void ReplaceMeshModelMatrices(glm::mat4 modelMatrix)
	{
		for (unsigned int i = 0; i < meshes.size(); i++)
		{
			meshes[i].transformObject.modelMatrix = modelMatrix;
		}
	}
	
	void NormalizeMeshes()
	{
		for (unsigned int i = 0; i < meshes.size(); i++)
		{
			meshes[i].Normalize();
		}
	}

	// vrati bounding box cijelog objekta
	std::vector<float> GetBoundingBox()
	{
		std::vector<float> boundingBox;
		float xmin, xmax, ymin, ymax, zmin, zmax;

		xmin = meshes[0].vertices[0].Position.x;
		xmax = meshes[0].vertices[0].Position.x;
		ymin = meshes[0].vertices[0].Position.y;
		ymax = meshes[0].vertices[0].Position.y;
		zmin = meshes[0].vertices[0].Position.z;
		zmax = meshes[0].vertices[0].Position.z;

		for (unsigned int i = 0; i < meshes.size(); i++)
		{
			for (unsigned int j = 0; j < meshes[i].vertices.size(); j++)
			{
				if (meshes[i].vertices[j].Position.x > xmax) xmax = meshes[i].vertices[j].Position.x;
				else if (meshes[i].vertices[j].Position.x < xmin) xmin = meshes[i].vertices[j].Position.x;
				if (meshes[i].vertices[j].Position.y > ymax) ymax = meshes[i].vertices[j].Position.y;
				else if (meshes[i].vertices[j].Position.y < ymin) ymin = meshes[i].vertices[j].Position.y;
				if (meshes[i].vertices[j].Position.z > zmax) zmax = meshes[i].vertices[j].Position.z;
				else if (meshes[i].vertices[j].Position.z < xmin) zmin = meshes[i].vertices[j].Position.z;
			}
		}

		boundingBox.push_back(xmin);
		boundingBox.push_back(xmax);
		boundingBox.push_back(ymin);
		boundingBox.push_back(ymax);
		boundingBox.push_back(zmin);
		boundingBox.push_back(zmax);
		
		return boundingBox;
	}

	

private:
	// ucitava aiScene, prolazi kroz sve cvorove i iz njih vadi mesheve
	void LoadModel(std::string const path)
	{
		// read file via ASSIMP
		Assimp::Importer importer;
		const aiScene* scene = importer.ReadFile(path, // scene = ReadFile(glava.obj)
			aiProcess_CalcTangentSpace | // kalkulira tangente i bitangente importiranih mesheva, ne radi ako mesh nema normale
			aiProcess_Triangulate | // poligone s vise vrhova pretvara u trokute
			aiProcess_JoinIdenticalVertices | // koristen kod indeksiranog pristupa, vise ploha moze koristiti isti vrh
			aiProcess_SortByPType | // podijeli mesh sa vise razlicitih primitiva u zasebne homogene mesheve
			aiProcess_FixInfacingNormals); // popravlja normale koje gledaju unutar objekta
		// check for errors
		if (!scene || !scene->mRootNode) // if is Not Zero
		{
			std::cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << "\n";
			return;
		}

		// process ASSIMP's root node recursively
		ProcessNode(scene->mRootNode, scene);
	}

	// za svaki node procesiraj mesheve
	void ProcessNode(aiNode* node, const aiScene* scene)
	{
		// za svaki mesh stvoriti Mesh objekt i spremiti ga u vector<Mesh> meshes
		for (unsigned int i = 0; i < node->mNumMeshes; i++)
		{
			aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
			meshes.push_back(ProcessMesh(mesh, scene));
		}
		// nakon obradenog cvora prelazimo na njegovu djecu
		for (unsigned int i = 0; i < node->mNumChildren; i++)
		{
			ProcessNode(node->mChildren[i], scene);
		}

	}

	// stvori nas custom Mesh iz aiMesh objekta
	Mesh ProcessMesh(aiMesh* mesh, const aiScene* scene)
	{
		std::vector<Vertex> vertices;
		std::vector<unsigned int> indices;
		// koordinate vrhova
		for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
			Vertex newVertex;
			newVertex.Position.x = mesh->mVertices[i].x;
			newVertex.Position.y = mesh->mVertices[i].y;
			newVertex.Position.z = mesh->mVertices[i].z;
			vertices.push_back(newVertex);
		}
		// indeksi vrhova, dohvacamo ih za svaku plohu
		for (unsigned int i = 0; i < mesh->mNumFaces; i++)
		{
			aiFace face = mesh->mFaces[i];
			for (unsigned int j = 0; j < face.mNumIndices; j++)
			{
				indices.push_back(face.mIndices[j]);
			}
		}

		return Mesh(vertices, indices);
	}
};
















// =========== KLASA KAMERA ================
class Camera : public Transform {
public:
	glm::vec3 ociste; // pozicija kamere
	glm::vec3 glediste; // tocka u koju je kamera usmjerena
	glm::vec3 smjer; // pravac od ocista do gledista
	glm::vec3 viewUp; // view-up vektor
	glm::vec3 rightAxis; // (lokalna) desna os kamere
	glm::vec3 upAxis; // (lokalna) gornja os kamere
	// omogucuje roll kamere, ali origigi view-up vektor ostaje isti da se zadrzi prirodna kretnja kamere
	// roll kamere ne radi full pa je trenutno disablean
	glm::vec3 rollViewUp; 
	
	float speed;
	float pitch = 0.0f;
	float yaw = -90.0f;

	Camera(glm::vec3 ociste, glm::vec3 glediste, glm::vec3 viewUp, float speed)
	{
		this->ociste = ociste;
		this->glediste = glediste;
		this->viewUp = viewUp;
		this->speed = speed;
		this->rollViewUp = viewUp;

		smjer.x = 0.0f; // cos(glm::radians(yaw) * cos(glm::radians(pitch)));
		smjer.y = 0.0f; // sin(glm::radians(pitch));
		smjer.z = -1.0f; // sin(glm::radians(yaw) * cos(glm::radians(pitch)));

		rightAxis = glm::normalize(glm::cross(viewUp, smjer)); // gleda lijevo
		upAxis = glm::normalize(glm::cross(smjer, rightAxis));
	}

	// konstruktor bez viewUp vektora (default = 0, 1, 0)
	Camera(glm::vec3 ociste, glm::vec3 glediste, float speed)
	{
		this->ociste = ociste;
		this->glediste = glediste;
		this->viewUp = glm::vec3(0.0f, 1.0f, 0.0f);
		this->rollViewUp = glm::vec3(0.0f, 1.0f, 0.0f);
		this->speed = speed;

		smjer.x = 0.0f; // cos(glm::radians(yaw) * cos(glm::radians(pitch)));
		smjer.y = 0.0f; // sin(glm::radians(pitch));
		smjer.z = -1.0f; // sin(glm::radians(yaw) * cos(glm::radians(pitch)));

		rightAxis = glm::normalize(glm::cross(viewUp, smjer)); // gleda lijevo
		upAxis = glm::normalize(glm::cross(smjer, rightAxis));
	}

	// s obzirom na pitch i yaw normalizira smjer, rightAxis i upAxis
	void UpdateSmjer()
	{
		glm::vec3 unnormalizedSmjer;
		unnormalizedSmjer.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
		unnormalizedSmjer.y = sin(glm::radians(pitch));
		unnormalizedSmjer.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));

		smjer = glm::normalize(unnormalizedSmjer);
		rightAxis = glm::normalize(glm::cross(viewUp, smjer));
		upAxis = glm::normalize(glm::cross(smjer, rightAxis));
	}

	// stvara perspektivnu matricu (koja skalira objekte u [-1, 1] s obzirom na NDC)
	glm::mat4 CreatePerspectiveMatrix(int width, int height, float fov)
	{
		glm::mat4 perspectiveMatrix = glm::perspective(fov, (float)width / (float)height, 0.1f, 100.0f);
		return perspectiveMatrix;
	}

	// stvara matricu pogleda s obzirom na polozaj kamere (matrica pogleda translatira objekte suprotno od kretnje kamere)
	glm::mat4 CreateViewMatrix()
	{
		glm::mat4 view = glm::lookAt(ociste, ociste + smjer, viewUp);
		return view;
	}
};
















// =========== KLASA RENDERER ================
class Renderer {
public:
	std::vector<Object*> objects;

	// default konstruktor
	Renderer() {};

	// konstruktor s vec gotovim vektorom objekata
	Renderer(std::vector<Object*> objects)
	{
		this->objects = objects;
	}

	// onak, glavna funkcija
	void Draw(Shader* shader)
	{
		for (unsigned int i = 0; i < objects.size(); i++)
		{
			objects[i]->Draw(shader);
		}
	}
};
















// =========== KLASA CURVE ================
class Curve
{
private:
	struct Segment
	{
		std::vector<Vertex> p_t;
		std::vector<glm::vec3> tangent_points;
		std::vector<glm::vec3> normal_points;
		std::vector<glm::mat3> R_matrices;
	};

	// glm stvara matricu stupac po stupac (a ne redak po redak)
	glm::mat4 B = glm::mat4(glm::vec4(-1, 3, -3, 1), 
							glm::vec4(3, -6, 0, 4), 
							glm::vec4(-3, 3, 3, 1), 
							glm::vec4(1, 0, 0, 0));

	glm::mat4x3 B_tangent = glm::mat4x3(glm::vec3(-1, 2, -1),
							glm::vec3(3, -4, 0),
							glm::vec3(-3, 2, 1),
							glm::vec3(1, 0, 0));

	void CalculateNewSegmentPoints(int index, int divisions)
	{
		Segment newSegment;
		

		for (int i = 0; i < divisions + 1; i++)
		{
			float t = (i * 1.0f) / divisions;
			
			// tocka krivulje
			glm::vec4 T = glm::vec4(t * t * t, t * t, t, 1);
			glm::mat3x4 R = glm::mat3x4(glm::vec4(controlPoints[index-1].x, controlPoints[index].x, controlPoints[index+1].x, controlPoints[index+2].x),
										glm::vec4(controlPoints[index - 1].y, controlPoints[index].y, controlPoints[index + 1].y, controlPoints[index + 2].y),
										glm::vec4(controlPoints[index - 1].z, controlPoints[index].z, controlPoints[index + 1].z, controlPoints[index + 2].z));
			glm::vec3 calculatedPoint = T * (1.0f / 6.0f) * B * R;

			Vertex newPoint;
			newPoint.Position.x = calculatedPoint.x;
			newPoint.Position.y = calculatedPoint.y;
			newPoint.Position.z = calculatedPoint.z;
			newSegment.p_t.push_back(newPoint);

			// tangenta
			glm::vec3 T_tangent = glm::vec3(t * t, t, 1);
			glm::vec3 tangent = T_tangent * (1.0f / 2.0f) * B_tangent * R;
			tangent = glm::normalize(tangent);
			newSegment.tangent_points.push_back(calculatedPoint);
			newSegment.tangent_points.push_back(calculatedPoint + 2.0f * tangent);	

			// normala
			glm::vec4 T_normal = glm::vec4(6 * t, 2, 0, 0);
			glm::vec3 second_derivation = T_normal * (1.0f / 6.0f) * B * R;
			glm::vec3 normal = glm::normalize(glm::cross(tangent, second_derivation));
			//glm::vec3 normal = glm::normalize(glm::cross(second_derivation, tangent));
			newSegment.normal_points.push_back(calculatedPoint);
			newSegment.normal_points.push_back(calculatedPoint + normal);

			// binormala
			glm::vec3 binormal = glm::cross(tangent, normal);

			// wuv matrica rotacija (DCM, ali wuv je vise fora naziv)
			// stupci: tangenta w, normala u, binormala v
			glm::mat3 R_matrix = glm::mat3(tangent, binormal, normal);
			newSegment.R_matrices.push_back(R_matrix);
		}

		curveSegments.push_back(newSegment);

	}

public:
	// tocke koje korisnik definira
	std::vector<glm::vec3> controlPoints;

	// vektor segmenata
	std::vector<Segment> curveSegments;

	GLuint VAO;
	GLuint VBO;

	Curve()
	{
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		std::cout << "Curve created.\n";
	}

	void AddPoint(glm::vec3 point)
	{
		controlPoints.push_back(point);
		if (controlPoints.size() > 3)
		{
			CalculateNewSegmentPoints(controlPoints.size() - 3, DIVISIONS);
		}
	}

	void DrawSegments(Shader* shader)
	{
		glUseProgram(shader->ID);
		glLineWidth(3);
		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
		glEnableVertexAttribArray(0);
		
		// kontrolni poligon
		if (crtajKontrolniPoligon)
		{
			glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * controlPoints.size(), &controlPoints[0], GL_DYNAMIC_DRAW);
			shader->setUniformVec3("curveColor", glm::vec3(0.0f, 0.0f, 0.0f));
			glDrawArrays(GL_LINE_STRIP, 0, controlPoints.size());
		}

		for (int i = 0; i < curveSegments.size(); i++)
		{
			// segmenti
			if (crtajKrivulju)
			{
				shader->setUniformVec3("curveColor", glm::vec3(1.0f, 0.0f, 0.0f));
				glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * curveSegments[i].p_t.size(), &curveSegments[i].p_t[0], GL_STATIC_DRAW);
				glDrawArrays(GL_LINE_STRIP, 0, curveSegments[i].p_t.size());
			}

			// tangente
			if (crtajTangente)
			{
				shader->setUniformVec3("curveColor", glm::vec3(0.0f, 1.0f, 0.0f));
				glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * curveSegments[i].tangent_points.size(), &curveSegments[i].tangent_points[0], GL_STATIC_DRAW);
				glDrawArrays(GL_LINES, 0, curveSegments[i].tangent_points.size());
			}

			// normale
			if (crtajNormale)
			{
				shader->setUniformVec3("curveColor", glm::vec3(0.0f, 0.0f, 1.0f));
				glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * curveSegments[i].normal_points.size(), &curveSegments[i].normal_points[0], GL_STATIC_DRAW);
				glDrawArrays(GL_LINES, 0, curveSegments[i].normal_points.size());
			}
		}
		glLineWidth(1);
		glBindVertexArray(0);
	}
};
















// global variables
int width = 900, height = 600;
float lastXpos = width / 2;
float lastYpos = height / 2;
float delta[2] = { 0.0f, 0.0f };
Camera sceneCamera(glm::vec3(0.0f, 0.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), 0.1f);



int main(int argc, char* argv[])
{

	srand((unsigned)time(NULL));
	std::cout << argv[0] << std::endl;

	// =========== PROZOR ================
	glfwInit();
	GLFWwindow* window;
	window = glfwCreateWindow(width, height, "RA_lab1", nullptr, nullptr);
	// Check for Valid Context
	if (window == nullptr) {
		fprintf(stderr, "Failed to Create OpenGL Context");
		exit(EXIT_FAILURE);
	}

	FPSManager FPSManagerObject(window, FPS, 1.0, "RA_lab1");

	glfwMakeContextCurrent(window);
	gladLoadGL();
	
	glClearColor(1, 1, 1, 1); //boja brisanja platna izmedu iscrtavanja dva okvira
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback); //funkcija koja se poziva prilikom mijenjanja velicine prozora
	fprintf(stderr, "OpenGL %s\n", glGetString(GL_VERSION));
	
	//glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_TRUE);
	//glfwSwapInterval(0); // vsync
	
	std::cout << "Kreiran window.\n";
















	// =========== SHADER ================
	Shader* shader[2];
	shader[0] = loadShader(argv[0], "shader1");
	shader[1] = loadShader(argv[0], "shader2");
	glUseProgram(shader[0]->ID);
	glUseProgram(shader[1]->ID);





















	// =========== UCITAVANJE OBJEKTA ================
	Assimp::Importer importer;
	std::string path(argv[0]);
	std::string dirPath(path, 0, path.find_last_of("\\/"));
	std::string resPath(dirPath);
	resPath.append("\\resources"); //za linux pretvoriti u forwardslash
	std::string objPath(resPath);
	//objPath.append("\\kocka.obj"); //za linux pretvoriti u forwardslash
	//objPath.append("\\glava.obj"); 
	//objPath.append("\\teddy.obj
	objPath.append("\\frog\\frog.obj");
	//objPath.append("\\snorlax.obj");

	const aiScene* scene = importer.ReadFile(objPath.c_str(), // scene = ReadFile(glava.obj)
		aiProcess_CalcTangentSpace | // kalkulira tangente i bitangente importiranih mesheva, ne radi ako mesh nema normale
		aiProcess_Triangulate | // poligone s vise vrhova pretvara u trokute
		aiProcess_JoinIdenticalVertices | // koristen kod indeksiranog pristupa, vise ploha moze koristiti isti vrh
		aiProcess_SortByPType | // podijeli mesh sa vise razlicitih primitiva u zasebne homogene mesheve
		aiProcess_FixInfacingNormals); // popravlja normale koje gledaju unutar objekta 

	if (!scene) {
		std::cerr << importer.GetErrorString();
		return false;
	}

	Object myObject(objPath);
	Renderer sceneRenderer;
	sceneRenderer.objects.push_back(&myObject);
	Transform transformObject;

	for (int i = 0; i < sceneRenderer.objects.size(); i++)
	{
		std::vector<float> boundingBox = sceneRenderer.objects[i]->GetBoundingBox();
		float xmin, xmax, ymin, ymax, zmin, zmax, xmid, ymid, zmid, M;
		xmin = boundingBox[0];
		xmax = boundingBox[1];
		ymin = boundingBox[2];
		ymax = boundingBox[3];
		zmin = boundingBox[4];
		zmax = boundingBox[5];

		xmid = (xmax + xmin) / (2);
		ymid = (ymax + ymin) / (2);
		zmid = (zmax + zmin) / (2);

		M = std::max(xmax - xmin, ymax - ymin);
		M = std::max(M, zmax - zmin);	

		// normalizacija objekta
		glm::mat4 translation = transformObject.Translation(glm::vec3(-xmid, -ymid, -zmid));
		glm::mat4 scaling = transformObject.Scaling(glm::vec3(2 / M, 2 / M, 2 / M));
		sceneRenderer.objects[i]->UpdateMeshNormalMatrices(translation);
		sceneRenderer.objects[i]->UpdateMeshNormalMatrices(scaling);
		sceneRenderer.objects[i]->NormalizeMeshes();
	}

































	// =========== KAMERA ================
	//                                                                               fov
	glm::mat4 perspectiveMatrix = sceneCamera.CreatePerspectiveMatrix(width, height, 45.0f);
	glm::mat4 viewMatrix = sceneCamera.CreateViewMatrix();

	for (unsigned int i = 0; i < sceneRenderer.objects.size(); i++)
	{
		sceneRenderer.objects[i]->UpdateMeshPerspectiveMatrices(perspectiveMatrix);
		sceneRenderer.objects[i]->UpdateMeshViewMatrices(viewMatrix);
	}

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	if (glfwRawMouseMotionSupported())
		glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);

	float lastXpos = width / 2;
	float lastYpos = height / 2;

	shader[1]->setUniformMat4("perspective", perspectiveMatrix);
	shader[1]->setUniformMat4("view", viewMatrix);

	glfwSetKeyCallback(window, key_callback);

	int pointCnt = 0;

	std::cout << "doso do whilea\n";


















	





	// =========================== KURVATURA ===========================
	Curve krivulja;
	krivulja.AddPoint(glm::vec3(10.0f, 0.0f, -10.0f));
	krivulja.AddPoint(glm::vec3(10.0f, 10.0f, -10.0f));
	krivulja.AddPoint(glm::vec3(0.0f, 10.0f, -10.0f));
	krivulja.AddPoint(glm::vec3(0.0f, 0.0f, -10.0f));
	krivulja.AddPoint(glm::vec3(-10.0f, 0.0f, -10.0f));
	krivulja.AddPoint(glm::vec3(-10.0f, 10.0f, -10.0f));



























	// varijable za animaciju
	bool bAnimacijaSeIzvrsava = false;
	int trenutniSegmentKrivulje = 0;
	int trenutnaDivizijaSegmenta = 0;

	//glm::mat4 matricaInicijalneRotacijeObjekta = transformObject.Rotation('y', 90);
	glm::mat4 matricaInicijalneRotacijeObjekta = glm::mat4(1.0f);


	// =========== GLAVNA PETLJA ================
	while (glfwWindowShouldClose(window) == false) {

		float deltaTime = (float)FPSManagerObject.enforceFPS(true);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// kontrola kamere pomocu tipkovnice
		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
			sceneCamera.ociste += sceneCamera.speed * sceneCamera.smjer;
			sceneCamera.glediste += sceneCamera.speed * sceneCamera.smjer;
		}
		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
			sceneCamera.ociste -= sceneCamera.speed * sceneCamera.smjer;
			sceneCamera.glediste -= sceneCamera.speed * sceneCamera.smjer;
		}
		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
			sceneCamera.ociste += sceneCamera.speed * glm::normalize(glm::cross(sceneCamera.smjer, sceneCamera.upAxis));
			sceneCamera.glediste += sceneCamera.speed * glm::normalize(glm::cross(sceneCamera.smjer, sceneCamera.upAxis));
		}
		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
			sceneCamera.ociste -= sceneCamera.speed * glm::normalize(glm::cross(sceneCamera.smjer, sceneCamera.upAxis));
			sceneCamera.glediste -= sceneCamera.speed * glm::normalize(glm::cross(sceneCamera.smjer, sceneCamera.upAxis));
		}
		if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {
			sceneCamera.ociste -= sceneCamera.speed * glm::normalize(glm::cross(sceneCamera.smjer, sceneCamera.rightAxis));
			sceneCamera.glediste -= sceneCamera.speed * glm::normalize(glm::cross(sceneCamera.smjer, sceneCamera.rightAxis));
		}
		if (glfwGetKey(window, GLFW_KEY_LEFT_ALT) == GLFW_PRESS) {
			sceneCamera.ociste += sceneCamera.speed * glm::normalize(glm::cross(sceneCamera.smjer, sceneCamera.rightAxis));
			sceneCamera.glediste += sceneCamera.speed * glm::normalize(glm::cross(sceneCamera.smjer, sceneCamera.rightAxis));
		}
		
		if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
			sceneCamera.speed = 0.3f;
		if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_RELEASE)
			sceneCamera.speed  = 0.1f;

		if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
		{
			myObject.ReplaceMeshModelMatrices(glm::mat4(1.0f));
		}

		// animacija
		if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS)
		{
			if (!bAnimacijaSeIzvrsava)
			{
				bAnimacijaSeIzvrsava = true;
				trenutniSegmentKrivulje = 0;
				trenutnaDivizijaSegmenta = 0;
			}
		}
		if (bAnimacijaSeIzvrsava) {
			// 1.0f oznacava jedinicnu, ak nema 1.0f, matrica je popunjena nulama - objekt nestaje
			glm::mat4 matricaPomakaNaKrivulju = glm::translate(glm::mat4(1.0f), krivulja.curveSegments[trenutniSegmentKrivulje].p_t[trenutnaDivizijaSegmenta].Position);
			//glm::mat3 matricaRotacije = glm::inverse(krivulja.curveSegments[trenutniSegmentKrivulje].R_matrices[trenutnaDivizijaSegmenta]);
			glm::mat4 matricaRotacije = glm::mat4(glm::inverse(krivulja.curveSegments[trenutniSegmentKrivulje].R_matrices[trenutnaDivizijaSegmenta]));
			//glm::mat4 matricaRotacije = glm::mat4(krivulja.curveSegments[trenutniSegmentKrivulje].R_matrices[trenutnaDivizijaSegmenta]);
			myObject.ReplaceMeshModelMatrices(matricaPomakaNaKrivulju * matricaRotacije * matricaInicijalneRotacijeObjekta);

			trenutnaDivizijaSegmenta++;
			if (trenutnaDivizijaSegmenta >= krivulja.curveSegments[trenutniSegmentKrivulje].p_t.size())
			{
				trenutniSegmentKrivulje++;
				trenutnaDivizijaSegmenta = 0;
			}
			if (trenutniSegmentKrivulje >= krivulja.curveSegments.size())
			{
				bAnimacijaSeIzvrsava = false;
			}
			
		}
		
		//myObject.ReplaceMeshModelMatrices(matricaInicijalneRotacijeObjekta);

		// kontrola kamere pomocu misa
		glfwSetCursorPosCallback(window, mouse_callback);
		sceneCamera.yaw += delta[0];
		sceneCamera.pitch += delta[1];
		delta[0] = 0.0f;
		delta[1] = 0.0f;

		if (sceneCamera.pitch > 89.0f) sceneCamera.pitch = 89.0f;
		if (sceneCamera.pitch < -89.0f) sceneCamera.pitch = -89.0f;

		sceneCamera.UpdateSmjer();
		
		viewMatrix = sceneCamera.CreateViewMatrix();
		shader[1]->setUniformMat4("view", viewMatrix);
		for (unsigned int i = 0; i < sceneRenderer.objects.size(); i++)
			sceneRenderer.objects[i]->UpdateMeshViewMatrices(viewMatrix);
		sceneRenderer.Draw(shader[0]);
		krivulja.DrawSegments(shader[1]);


		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();

	return EXIT_SUCCESS;
}

// ucitava vertex shader i fragment shader istog imena
Shader* loadShader(char* path, char* naziv) {
	std::string sPath(path);
	std::string pathVert;
	std::string pathFrag;
	std::string pathGeom;

	pathVert.append(path, sPath.find_last_of("\\/") + 1);
	pathFrag.append(path, sPath.find_last_of("\\/") + 1);
	pathGeom.append(path, sPath.find_last_of("\\/") + 1);
	if (pathFrag[pathFrag.size() - 1] == '/') {
		pathVert.append("shaders/");
		pathFrag.append("shaders/");
		pathGeom.append("shaders/");
	}
	else if (pathFrag[pathFrag.size() - 1] == '\\') {
		pathVert.append("shaders\\");
		pathFrag.append("shaders\\");
		pathGeom.append("shaders\\");
	}
	else {
		std::cerr << "nepoznat format pozicije shadera";
		exit(1);
	}

	pathVert.append(naziv);
	pathVert.append(".vert");
	pathFrag.append(naziv);
	pathFrag.append(".frag");
	pathGeom.append(naziv);
	pathGeom.append(".geom");

	//return new Shader(pathVert.c_str(), pathFrag.c_str(), nullptr);
	return new Shader(pathVert.c_str(), pathFrag.c_str(), pathGeom.c_str());
}

void framebuffer_size_callback(GLFWwindow* window, int Width, int Height)
{
	width = Width;
	height = Height;
	glViewport(0, 0, width, height);
	lastXpos = width / 2;
	lastYpos = height / 2;
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	float xPosFloat = (float)xpos;
	float yPosFloat = (float)ypos;

	float deltaX = xPosFloat - lastXpos;
	float deltaY = yPosFloat - lastYpos;

	lastXpos = xPosFloat;
	lastYpos = yPosFloat;

	//float sensitivity = 1.5f;
	float sensitivity = 1.1f;
	deltaX *= sensitivity;
	deltaY *= sensitivity;

	delta[0] = deltaX;
	delta[1] = -deltaY;
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	// ESCAPE izlazi iz prozora
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
	
	// R rastavi kameru na proste faktore
	if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
	{
		sceneCamera.ociste = glm::vec3(0.0f, 0.0f, 2.0f);
		sceneCamera.glediste = glm::vec3(0.0f, 0.0f, 0.0f);

		glm::vec3 resetSmjer;
		resetSmjer.x = 0.0f; // cos(glm::radians(-90.0f))* cos(glm::radians(0.0f));
		resetSmjer.y = 0.0f; // sin(glm::radians(0.0f));
		resetSmjer.z = -1.0f; // sin(glm::radians(-90.0f))* cos(glm::radians(0.0f));

		sceneCamera.yaw = -90.0f;
		sceneCamera.pitch = 0.0f;
		sceneCamera.viewUp = glm::vec3(0.0f, 1.0f, 0.0f);
		sceneCamera.rollViewUp = glm::vec3(0.0f, 1.0f, 0.0f);
		sceneCamera.smjer = resetSmjer;
	}

}