#define _CRT_SECURE_NO_DEPRECATE
#include <GL/freeglut.h>
#include <math.h>
#include <stdio.h>
#include <iostream>
#include <string> 


//Set the width and height of the Window
#define Windowwidth  1024
#define Windowheight 1024
// Here the Distance can be treated as a unit. For example, the distance between  
// mercurio and sun is 1*Distance, and between earth and sun is 3*Distance
#define Distance 100000
//The radius of earth (expanded), can also be treated as unit to calcualte other planet's radius
#define Eradius 16000
const GLfloat Pi = 3.1415926536f;
int day = 0;
int t_day = 1;
int rewrite_day = 0;
int month = 0;
int year = 0;
std::string dayString;
GLfloat yaw = 80.0; // Initial yaw set to 0
GLdouble Angle = 80.0;
GLdouble aix_x = 930000.0, aix_y = -80000, aix_z = 2000000;
GLdouble cameraDistance = 0;

const float FPS = 60.0f;
const float degrees_per_frame = 1 / FPS;   //0.016666667;   // ---> 1/60 frames

int counter_frames = 0;
float speed_controll = 1.0f;

float get_rot_per_frame_year(float days_to_rotate) {

    float res = (360 / (days_to_rotate * speed_controll)) * degrees_per_frame;
    return res;
}

float get_rot_per_frame_day(float days_to_rotate) {
    float total_frames = days_to_rotate * FPS;
    float res = 360 / total_frames;
    return res;
}


//planets set up in order as they are
//SUN
float sunRotationAngle = 0.0f;
float suns_rotation_per_frame = get_rot_per_frame_year(27.0f);

//Mercury
float mercRotationAngle = 0.0f;
float merc_rotation_per_frame = get_rot_per_frame_year(88.0f);
float mercAxisRotationAngle = 0.0f;
float merc_spin_per_frame = get_rot_per_frame_day(58.6f); //how many times does the planet turn around y axis during one day - earth = 1

//Venus
float venusRotationAngle = 0.0f;
float venus_rotation_per_frame = get_rot_per_frame_year(225.0f);
float venusAxisRotationAngle = 0.0f;
float venus_spin_per_frame = get_rot_per_frame_day(243.0f);

//EARTH
float earthRotationAngle = 0.0f;
float earth_rotation_per_frame = get_rot_per_frame_year(365.0f);
float earthAxisRotationAngle = 0.0f;
float earth_spin_per_frame = get_rot_per_frame_day(0.99f); //how many times does the planet turn around y axis during one day - earth = 1

//MOON
float moonRotationAngle = 0.0f;
float moon_rotation_per_frame = get_rot_per_frame_year(27.3f);
float moonAxisRotationAngle = 0.0f;
float moon_spin_per_frame = get_rot_per_frame_day(273.0f); //how many times does the planet turn around y axis during one day - earth = 1

//Mars
float marsRotationAngle = 0.0f;
float mars_rotation_per_frame = get_rot_per_frame_year(687.0f);
float marsAxisRotationAngle = 0.0f;
float mars_spin_per_frame = get_rot_per_frame_day(1.02f);

//Jupiter
float jupiterRotationAngle = 0.0f;
float jupiter_rotation_per_frame = get_rot_per_frame_year(4331.0f);
float jupiterAxisRotationAngle = 0.0f;
float jupiter_spin_per_frame = get_rot_per_frame_day(0.41f);

//Saturn
float saturnRotationAngle = 0.0f;
float saturn_rotation_per_frame = get_rot_per_frame_year(10747.0f);
float saturnAxisRotationAngle = 0.0f;
float saturn_spin_per_frame = get_rot_per_frame_day(0.44f);

//Uranus
float uranusRotationAngle = 0.0f;
float uranus_rotation_per_frame = get_rot_per_frame_year(30589.0f);
float uranusAxisRotationAngle = 0.0f;
float uranus_spin_per_frame = get_rot_per_frame_day(0.71f);

//Neptune
float neptuneRotationAngle = 0.0f;
float neptune_rotation_per_frame = get_rot_per_frame_year(59800.0f);
float neptuneAxisRotationAngle = 0.0f;
float neptune_spin_per_frame = get_rot_per_frame_day(0.67f);

float updateRotation(float rot_per_frame, float RotationAngle) {

    // Rotation per frame
    //std::cout << "Rotations per frame: " << rot_per_frame << std::endl;
    float rotationPerFrame = rot_per_frame;

    // Update the Sun's rotation angle
    RotationAngle += rotationPerFrame;

    // Optional: Adjust the rotation angle to avoid overflow
    if (RotationAngle >= 360.0f) {
        RotationAngle -= 360.0f;
    }

    return RotationAngle;
}

float updateAxisRotation(float rot_per_frame, float RotationAngle) {

    // Rotation per frame
    //std::cout << "Rotations per frame: " << rot_per_frame << std::endl;
    float rotationPerFrame = rot_per_frame;

    // Update the Sun's rotation angle
    RotationAngle += rotationPerFrame;

    // Optional: Adjust the rotation angle to avoid overflow
    if (RotationAngle >= 360.0f) {
        RotationAngle -= 360.0f;
    }

    return RotationAngle;
}

//The ratio of radius0.4:1:1:0.5:11:9:4:3
//mercurios,venus,earth,moon,mars,jupiter,saturn,uranus,neptune 
GLuint tbg,tsun,tearth,tmoon,tmercu,tven,tmars,tjup,tsat,tura,tnep;

//Loading texture files, which can also be found at http://www.cppblog.com/doing5552/archive/2009/01/08/71532.aspx
int power_of_two(int n)
{
    if (n <= 0)
        return 0;
    return (n & (n - 1)) == 0;
}

GLuint LoadTexture(const char* filename)
{

    GLint width, height, total_bytes;
    GLubyte* pixels = 0;
    GLuint  texture_ID = 0;

    
    FILE* pFile = fopen(filename, "rb");
    if (pFile == 0)
        return 0;

    fseek(pFile, 0x0012, SEEK_SET);
    fread(&width, 4, 1, pFile);
    fread(&height, 4, 1, pFile);
    fseek(pFile, 54, SEEK_SET);

    
    {
        GLint line_bytes = width * 3;
        while (line_bytes % 4 != 0)
            ++line_bytes;
        total_bytes = line_bytes * height;
    }

    
    pixels = (GLubyte*)malloc(total_bytes);
    if (pixels == 0)
    {
        fclose(pFile);
        return 0;
    }

    
    if (fread(pixels, total_bytes, 1, pFile) <= 0)
    {
        free(pixels);
        fclose(pFile);
        return 0;
    }

    
    // The is for the compatibility with old version of opengl
    {
        GLint max;
        glGetIntegerv(GL_MAX_TEXTURE_SIZE, &max);
        if (!power_of_two(width)
            || !power_of_two(height)
            || width > max
            || height > max)
        {
            const GLint new_width = 512;
            const GLint new_height = 512; // set the picture to 512*512
            GLint new_line_bytes, new_total_bytes;
            GLubyte* new_pixels = 0;

            
            new_line_bytes = new_width * 3;
            while (new_line_bytes % 4 != 0)
                ++new_line_bytes;
            new_total_bytes = new_line_bytes * new_height;

            
            new_pixels = (GLubyte*)malloc(new_total_bytes);
            if (new_pixels == 0)
            {
                free(pixels);
                fclose(pFile);
                return 0;
            }

            
            gluScaleImage(GL_RGB,
                width, height, GL_UNSIGNED_BYTE, pixels,
                new_width, new_height, GL_UNSIGNED_BYTE, new_pixels);

            
            free(pixels);
            pixels = new_pixels;
            width = new_width;
            height = new_height;
        }
    }

    
    glGenTextures(1, &texture_ID);
    if (texture_ID == 0)
    {
        free(pixels);
        fclose(pFile);
        return 0;
    }

    glBindTexture(GL_TEXTURE_2D, texture_ID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0,
        GL_BGR_EXT, GL_UNSIGNED_BYTE, pixels);
    free(pixels);
    return texture_ID;

}

//Initilize to load files
void init_LoadallTexture()
{
    tsun = LoadTexture("pictures/sol.bmp");
    tbg = LoadTexture("pictures/bg_planets.bmp");
    tearth = LoadTexture("pictures/terra.bmp");
    tmoon = LoadTexture("pictures/lua.bmp");
    tmercu = LoadTexture("pictures/mercurio.bmp");
    tven = LoadTexture("pictures/venus.bmp");
    tmars = LoadTexture("pictures/marte.bmp");
    tjup = LoadTexture("pictures/jupiter.bmp");
    tsat = LoadTexture("pictures/saturno.bmp");
    tura = LoadTexture("pictures/urano.bmp");
    tnep = LoadTexture("pictures/neptuno.bmp");
}

// Get every planet and background
//The transform of matrix firstly is set to achieve the Orbital revolution, then self-rotation 
//The parameters about lighting is specially set according to some references
//Quadratic surface to draw a sphere, which is convenient since the texture coordinate can directly used on sphere
void get_bg()
{ 
    glPushMatrix();
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    
    glOrtho(-1.0, 1.0, -1.0, 1.0, -1.0, 1.0); 
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, tbg);
    glBegin(GL_QUADS);
    glTexCoord2d(0.0, 0.0); glVertex2d(-1.0, -1.0);
    glTexCoord2d(1.0, 0.0); glVertex2d(+1.0, -1.0);
    glTexCoord2d(1.0, 1.0); glVertex2d(+1.0, +1.0);
    glTexCoord2d(0.0, 1.0); glVertex2d(-1.0, +1.0);
    glEnd();
    glDisable(GL_TEXTURE_2D);
    glPopMatrix();
}


void get_sun()
{
    glPushMatrix();
    
    //rotation
    sunRotationAngle = updateRotation(suns_rotation_per_frame, sunRotationAngle);
    glRotatef(sunRotationAngle, 0.0, 0.0, -1.0);

    {
        GLfloat sun_light_position[] = { 0.0f, 0.0f, 0.0f, 1.0f };
        GLfloat sun_light_ambient[] = { 0.0f, 0.0f, 0.0f, 1.0f };
        GLfloat sun_light_diffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };
        GLfloat sun_light_specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };

        glLightfv(GL_LIGHT0, GL_POSITION, sun_light_position);
        glLightfv(GL_LIGHT0, GL_AMBIENT, sun_light_ambient);
        glLightfv(GL_LIGHT0, GL_DIFFUSE, sun_light_diffuse);
        glLightfv(GL_LIGHT0, GL_SPECULAR, sun_light_specular);

        glEnable(GL_LIGHT0);
        glEnable(GL_LIGHTING);
        
    }
    {
        GLfloat sun_mat_ambient[] = { 0.2f, 0.2f, 0.2f, 1.0f };
        GLfloat sun_mat_diffuse[] = { 0.8f, 0.8f, 0.8f, 1.0f };
        GLfloat sun_mat_specular[] = { 0.0f, 0.0f, 0.0f, 1.0f };
        GLfloat sun_mat_emission[] = { 1.0f, 1.0f, 1.0f, 1.0f };
        GLfloat sun_mat_shininess = 0.0f;

        glMaterialfv(GL_FRONT, GL_AMBIENT, sun_mat_ambient);
        glMaterialfv(GL_FRONT, GL_DIFFUSE, sun_mat_diffuse);
        glMaterialfv(GL_FRONT, GL_SPECULAR, sun_mat_specular);
        glMaterialfv(GL_FRONT, GL_EMISSION, sun_mat_emission);
        glMaterialf(GL_FRONT, GL_SHININESS, sun_mat_shininess);
    }


    GLUquadricObj* sphere = NULL;
    sphere = gluNewQuadric();
    glEnable(GL_TEXTURE_2D);
    gluQuadricDrawStyle(sphere, GLU_FILL);
    glBindTexture(GL_TEXTURE_2D, tsun);
    gluQuadricTexture(sphere, GL_TRUE);
    gluQuadricNormals(sphere, GLU_SMOOTH);
    gluSphere(sphere,69600, 100, 100);
    glDisable(GL_TEXTURE_2D);
    glPopMatrix();
    counter_frames++;
}

void get_earth() {
    glPushMatrix(); // Save the current matrix state

    // First, rotate for the orbital motion
    earthRotationAngle = updateRotation(earth_rotation_per_frame, earthRotationAngle);
    glRotatef(earthRotationAngle, 0.0f, 0.0f, -1.0f); // Orbital rotation around the z-axis

    // Then, translate to the orbital position
    glTranslatef(3 * Distance, 0.0f, 0.0f); // Move Earth to its position in orbit

    // around its axis
    //glRotatef(6.0f, 0.0f, 1.0f, 0.0f); // Daily rotation around the Earth's axis
    earthAxisRotationAngle = updateAxisRotation(earth_spin_per_frame, earthAxisRotationAngle);
    glRotatef(earthAxisRotationAngle, 0.0f, 0.0f, -1.0f);

    //There will be a drift when it comes to rotation due to the fact that earth rotates around Y axis in less than 1 year. Thats why we have heap year. 


    // Set material properties and draw Earth
    {
        GLfloat earth_mat_ambient[] = { 0.2f, 0.2f, 0.2f, 1.0f };
        GLfloat earth_mat_diffuse[] = { 0.8f, 0.8f, 0.8f, 1.0f };
        GLfloat earth_mat_specular[] = { 0.0f, 0.0f, 0.0f, 1.0f };
        GLfloat earth_mat_emission[] = { 0.0f, 0.0f, 0.0f, 1.0f };
        GLfloat earth_mat_shininess = 0.0f;
        glMaterialfv(GL_FRONT, GL_AMBIENT, earth_mat_ambient);
        glMaterialfv(GL_FRONT, GL_DIFFUSE, earth_mat_diffuse);
        glMaterialfv(GL_FRONT, GL_SPECULAR, earth_mat_specular);
        glMaterialfv(GL_FRONT, GL_EMISSION, earth_mat_emission);
        glMaterialf(GL_FRONT, GL_SHININESS, earth_mat_shininess);
    }

    // Draw the Earth as a textured sphere
    GLUquadricObj* sphere = gluNewQuadric();
    glEnable(GL_TEXTURE_2D);
    gluQuadricDrawStyle(sphere, GLU_FILL);
    gluQuadricTexture(sphere, GL_TRUE);
    gluQuadricNormals(sphere, GLU_SMOOTH);
    glBindTexture(GL_TEXTURE_2D, tearth);
    gluSphere(sphere, 15945, 100, 100);
    glDisable(GL_TEXTURE_2D);

    glPushMatrix(); // Save Earth's matrix state for the Moon

    // ----------------- MOON -----------------

    // Moon's orbital motion around the Earth
    //moonRotationAngle = updateRotation(moon_rotation_per_frame, moonRotationAngle);
    //glRotatef(moonRotationAngle, 0.0f, 1.0f, 0.0f);

    // Translate Moon to its orbital radius from Earth
    glTranslatef(0.384 * Distance, 0.0f, 0.0f);

    //glRotatef(6.0f, 0.0f, 1.0f, 0.0f); // Daily rotation around the Earth's axis
    moonAxisRotationAngle = updateAxisRotation(moon_spin_per_frame, moonAxisRotationAngle);
    glRotatef(moonAxisRotationAngle, 0.0f, 1.0f, 0.0f);

    // Set material properties and draw the Moon
    GLfloat moon_mat_ambient[] = { 0.1f, 0.1f, 0.1f, 1.0f };
    GLfloat moon_mat_diffuse[] = { 0.6f, 0.6f, 0.6f, 1.0f };
    GLfloat moon_mat_specular[] = { 0.3f, 0.3f, 0.3f, 1.0f };
    GLfloat moon_mat_emission[] = { 0.0f, 0.0f, 0.0f, 1.0f };
    GLfloat moon_mat_shininess = 10.0f;
    glMaterialfv(GL_FRONT, GL_AMBIENT, moon_mat_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, moon_mat_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, moon_mat_specular);
    glMaterialfv(GL_FRONT, GL_EMISSION, moon_mat_emission);
    glMaterialf(GL_FRONT, GL_SHININESS, moon_mat_shininess);

    sphere = gluNewQuadric();
    glEnable(GL_TEXTURE_2D);
    gluQuadricDrawStyle(sphere, GLU_FILL);
    gluQuadricTexture(sphere, GL_TRUE);
    gluQuadricNormals(sphere, GLU_SMOOTH);
    glBindTexture(GL_TEXTURE_2D, tmoon);
    gluSphere(sphere, 4345, 50, 50);  // Adjust the size according to your scale
    glDisable(GL_TEXTURE_2D);

    glPopMatrix(); // Restore Earth's matrix state
    glPopMatrix(); // Restore the original matrix state}
}


void get_mercu()
{
    glPushMatrix();
    //rotation 
    mercRotationAngle = updateRotation(merc_rotation_per_frame, mercRotationAngle);
    glRotatef(mercRotationAngle, 0.0, 0.0, -1.0);

    glTranslatef(Distance, 0.0f, 0.0f);

    mercAxisRotationAngle = updateAxisRotation(merc_spin_per_frame, mercAxisRotationAngle);
    glRotatef(mercAxisRotationAngle, 0.0f, 0.0f, -1.0f);

    {
        GLfloat mat_ambient[] = { 0.2f, 0.2f, 0.2f, 1.0f };
        GLfloat mat_diffuse[] = { 0.8f, 0.8f, 0.8f, 1.0f };
        GLfloat mat_specular[] = { 0.0f, 0.0f, 0.0f, 1.0f };
        GLfloat mat_emission[] = { 0.0f, 0.0f, 0.0f, 1.0f };
        GLfloat mat_shininess = 0.0f;

        glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
        glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
        glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
        glMaterialfv(GL_FRONT, GL_EMISSION, mat_emission);
        glMaterialf(GL_FRONT, GL_SHININESS, mat_shininess);

    }
    GLUquadricObj* sphere = NULL;
    sphere = gluNewQuadric();
    glEnable(GL_TEXTURE_2D);
    gluQuadricDrawStyle(sphere, GLU_FILL);

    glBindTexture(GL_TEXTURE_2D, tmercu);
    gluQuadricTexture(sphere, GL_TRUE);
    gluQuadricNormals(sphere, GLU_SMOOTH);
    gluSphere(sphere, 0.4*Eradius, 100, 100);
    glDisable(GL_TEXTURE_2D);
    glPopMatrix();

}
void get_ven()
{
    glPushMatrix();
    //rotation
    venusRotationAngle = updateRotation(venus_rotation_per_frame, venusRotationAngle);
    glRotatef(venusRotationAngle, 0.0, 0.0, -1.0);

    glTranslatef(2 * Distance, 0.0f, 0.0f);

    venusAxisRotationAngle = updateAxisRotation(venus_spin_per_frame, venusAxisRotationAngle);
    glRotatef(venusAxisRotationAngle, 0.0f, 0.0f, -1.0f);
    {
        GLfloat mat_ambient[] = { 0.2f, 0.2f, 0.2f, 1.0f };
        GLfloat mat_diffuse[] = { 0.8f, 0.8f, 0.8f, 1.0f };
        GLfloat mat_specular[] = { 0.0f, 0.0f, 0.0f, 1.0f };
        GLfloat mat_emission[] = { 0.0f, 0.0f, 0.0f, 1.0f };
        GLfloat mat_shininess = 0.0f;

        glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
        glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
        glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
        glMaterialfv(GL_FRONT, GL_EMISSION, mat_emission);
        glMaterialf(GL_FRONT, GL_SHININESS, mat_shininess);

    }
    GLUquadricObj* sphere = NULL;
    sphere = gluNewQuadric();
    glEnable(GL_TEXTURE_2D);
    gluQuadricDrawStyle(sphere, GLU_FILL);

    glBindTexture(GL_TEXTURE_2D, tven);
    gluQuadricTexture(sphere, GL_TRUE);
    gluQuadricNormals(sphere, GLU_SMOOTH);
    gluSphere(sphere,  Eradius, 100, 100);
    glDisable(GL_TEXTURE_2D);
    glPopMatrix();

}
void get_mars()
{

    glPushMatrix();
    marsRotationAngle = updateRotation(mars_rotation_per_frame, marsRotationAngle);
    glRotatef(marsRotationAngle, 0.0, 0.0, -1.0);

    glTranslatef(4 * Distance, 0.0f, 0.0f);

    marsAxisRotationAngle = updateAxisRotation(mars_spin_per_frame, marsAxisRotationAngle);
    glRotatef(marsAxisRotationAngle, 0.0f, 0.0f, -1.0f);

    {
        GLfloat mat_ambient[] = { 0.2f, 0.2f, 0.2f, 1.0f };
        GLfloat mat_diffuse[] = { 0.8f, 0.8f, 0.8f, 1.0f };
        GLfloat mat_specular[] = { 0.0f, 0.0f, 0.0f, 1.0f };
        GLfloat mat_emission[] = { 0.0f, 0.0f, 0.0f, 1.0f };
        GLfloat mat_shininess = 0.0f;

        glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
        glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
        glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
        glMaterialfv(GL_FRONT, GL_EMISSION, mat_emission);
        glMaterialf(GL_FRONT, GL_SHININESS, mat_shininess);

    }
    GLUquadricObj* sphere = NULL;
    sphere = gluNewQuadric();
    glEnable(GL_TEXTURE_2D);
    gluQuadricDrawStyle(sphere, GLU_FILL);

    glBindTexture(GL_TEXTURE_2D, tmars);
    gluQuadricTexture(sphere, GL_TRUE);
    gluQuadricNormals(sphere, GLU_SMOOTH);
    gluSphere(sphere, 0.5*Eradius, 100, 100);
    glDisable(GL_TEXTURE_2D);
    glPopMatrix();

}
void get_jup()
{
    glPushMatrix();
    jupiterRotationAngle = updateRotation(jupiter_rotation_per_frame, jupiterRotationAngle);
    glRotatef(jupiterRotationAngle, 0.0, 0.0, -1.0);

    glTranslatef(6.5 * Distance, 0.0f, 0.0f);

    jupiterAxisRotationAngle = updateAxisRotation(jupiter_spin_per_frame, jupiterAxisRotationAngle);
    glRotatef(jupiterAxisRotationAngle, 0.0f, 0.0f, -1.0f);

    {
        GLfloat mat_ambient[] = { 0.2f, 0.2f, 0.2f, 1.0f };
        GLfloat mat_diffuse[] = { 0.8f, 0.8f, 0.8f, 1.0f };
        GLfloat mat_specular[] = { 0.0f, 0.0f, 0.0f, 1.0f };
        GLfloat mat_emission[] = { 0.0f, 0.0f, 0.0f, 1.0f };
        GLfloat mat_shininess = 0.0f;

        glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
        glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
        glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
        glMaterialfv(GL_FRONT, GL_EMISSION, mat_emission);
        glMaterialf(GL_FRONT, GL_SHININESS, mat_shininess);

    }
    GLUquadricObj* sphere = NULL;
    sphere = gluNewQuadric();
    glEnable(GL_TEXTURE_2D);
    gluQuadricDrawStyle(sphere, GLU_FILL);

    glBindTexture(GL_TEXTURE_2D, tjup);
    gluQuadricTexture(sphere, GL_TRUE);
    gluQuadricNormals(sphere, GLU_SMOOTH);
    gluSphere(sphere, 11*Eradius, 100, 100);
    glDisable(GL_TEXTURE_2D);
    glPopMatrix();

}
void get_sat()
{
    glPushMatrix();
 
    saturnRotationAngle = updateRotation(saturn_rotation_per_frame, saturnRotationAngle);
    glRotatef(saturnRotationAngle, 0.0, 0.0, -1.0);

    glTranslatef(10.5 * Distance, 0.0f, 0.0f);

    saturnAxisRotationAngle = updateAxisRotation(saturn_spin_per_frame, saturnAxisRotationAngle);
    glRotatef(saturnAxisRotationAngle, 0.0f, 0.0f, -1.0f);

    {
        GLfloat mat_ambient[] = { 0.2f, 0.2f, 0.2f, 1.0f };
        GLfloat mat_diffuse[] = { 0.8f, 0.8f, 0.8f, 1.0f };
        GLfloat mat_specular[] = { 0.0f, 0.0f, 0.0f, 1.0f };
        GLfloat mat_emission[] = { 0.0f, 0.0f, 0.0f, 1.0f };
        GLfloat mat_shininess = 0.0f;

        glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
        glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
        glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
        glMaterialfv(GL_FRONT, GL_EMISSION, mat_emission);
        glMaterialf(GL_FRONT, GL_SHININESS, mat_shininess);

    }
    GLUquadricObj* sphere = NULL;
    sphere = gluNewQuadric();
    glEnable(GL_TEXTURE_2D);
    gluQuadricDrawStyle(sphere, GLU_FILL);

    glBindTexture(GL_TEXTURE_2D, tsat);
    gluQuadricTexture(sphere, GL_TRUE);
    gluQuadricNormals(sphere, GLU_SMOOTH);
    gluSphere(sphere, 9*Eradius, 100, 100);
    glDisable(GL_TEXTURE_2D);
    glPopMatrix();

}
void get_ura()
{
    glPushMatrix();

    uranusRotationAngle = updateRotation(uranus_rotation_per_frame, uranusRotationAngle);
    glRotatef(uranusRotationAngle, 0.0, 0.0, -1.0);

    glTranslatef(13 * Distance, 0.0f, 0.0f);

    uranusAxisRotationAngle = updateAxisRotation(uranus_spin_per_frame, uranusAxisRotationAngle);
    glRotatef(uranusAxisRotationAngle, 0.0f, 0.0f, -1.0f);

    {
        GLfloat mat_ambient[] = { 0.2f, 0.2f, 0.2f, 1.0f };
        GLfloat mat_diffuse[] = { 0.8f, 0.8f, 0.8f, 1.0f };
        GLfloat mat_specular[] = { 0.0f, 0.0f, 0.0f, 1.0f };
        GLfloat mat_emission[] = { 0.0f, 0.0f, 0.0f, 1.0f };
        GLfloat mat_shininess = 0.0f;

        glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
        glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
        glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
        glMaterialfv(GL_FRONT, GL_EMISSION, mat_emission);
        glMaterialf(GL_FRONT, GL_SHININESS, mat_shininess);

    }
    GLUquadricObj* sphere = NULL;
    sphere = gluNewQuadric();
    glEnable(GL_TEXTURE_2D);
    gluQuadricDrawStyle(sphere, GLU_FILL);

    glBindTexture(GL_TEXTURE_2D, tura);
    gluQuadricTexture(sphere, GL_TRUE);
    gluQuadricNormals(sphere, GLU_SMOOTH);
    gluSphere(sphere, 4*Eradius, 100, 100);
    glDisable(GL_TEXTURE_2D);
    glPopMatrix();

}
void get_nep()
{
    glPushMatrix();

    neptuneRotationAngle = updateRotation(neptune_rotation_per_frame, neptuneRotationAngle);
    glRotatef(neptuneRotationAngle, 0.0, 0.0, -1.0);

    glTranslatef(15 * Distance, 0.0f, 0.0f);

    neptuneAxisRotationAngle = updateAxisRotation(neptune_spin_per_frame, neptuneAxisRotationAngle);
    glRotatef(neptuneAxisRotationAngle, 0.0f, 0.0f, -1.0f);

    {
        GLfloat mat_ambient[] = { 0.2f, 0.2f, 0.2f, 1.0f };
        GLfloat mat_diffuse[] = { 0.8f, 0.8f, 0.8f, 1.0f };
        GLfloat mat_specular[] = { 0.0f, 0.0f, 0.0f, 1.0f };
        GLfloat mat_emission[] = { 0.0f, 0.0f, 0.0f, 1.0f };
        GLfloat mat_shininess = 0.0f;

        glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
        glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
        glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
        glMaterialfv(GL_FRONT, GL_EMISSION, mat_emission);
        glMaterialf(GL_FRONT, GL_SHININESS, mat_shininess);

    }
    GLUquadricObj* sphere = NULL;
    sphere = gluNewQuadric();
    glEnable(GL_TEXTURE_2D);
    gluQuadricDrawStyle(sphere, GLU_FILL);

    glBindTexture(GL_TEXTURE_2D, tnep);
    gluQuadricTexture(sphere, GL_TRUE);
    gluQuadricNormals(sphere, GLU_SMOOTH);
    gluSphere(sphere, 3*Eradius, 100, 100);
    glDisable(GL_TEXTURE_2D);
    glPopMatrix();
    

}

void renderBitmapString(float x, float y, void* font, const char* string) {
    const char* c;
    glRasterPos2f(x, y);
    for (c = string; *c != '\0'; c++) {
        glutBitmapCharacter(font, *c);
    }
}

void DisplayText(int day) {
        // Save current matrix state and attributes
        glPushAttrib(GL_ENABLE_BIT);
        glMatrixMode(GL_PROJECTION);
        glPushMatrix();
        glLoadIdentity();
        int width = glutGet(GLUT_WINDOW_WIDTH);
        int height = glutGet(GLUT_WINDOW_HEIGHT);
        gluOrtho2D(0, width, 0, height);

        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
        glLoadIdentity();

        // Disable depth test so text is not removed by depth buffer
        glDisable(GL_DEPTH_TEST);

        // Set text properties
        glColor3f(1.0, 1.0, 1.0);  // White color

        
        

        if (t_day > 30) {
            ++month;
            t_day = 1;
        }
        else if (month == 12 && t_day == 30) {
            year++;
            t_day = 1;
            month = 1;
        }
 
        if (month >= 1 && year == 0) {
            dayString = "Day: " + std::to_string(t_day) + " Month: " + std::to_string(month);
        }
        else if (year >= 1) {
            dayString = "Day: " + std::to_string(t_day) + " Month: " + std::to_string(month) + " Year: " + std::to_string(year);
        }
        else {
            dayString = "Day: " + std::to_string(t_day);

        }
        const char* cstr = dayString.c_str();  // Convert string to C-style string

        // Adjust these coordinates as needed
        renderBitmapString(20, 20, GLUT_BITMAP_TIMES_ROMAN_24, cstr);  // Position text

        // Restore matrices
        glPopMatrix(); 
        glMatrixMode(GL_PROJECTION);
        glPopMatrix();
        glMatrixMode(GL_MODELVIEW);

        // Re-enable depth test if it was enabled previously
        glPopAttrib();
}

//Display Function
void myDisplay(void)
{ 
    int width = glutGet(GLUT_WINDOW_WIDTH);
    int height = glutGet(GLUT_WINDOW_HEIGHT);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    get_bg();
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(Angle, (float)width / (float)height, 1, 10000000);    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    GLfloat dir_x = cos(yaw);  // Assuming yaw is in radians and camera looks towards the origin
    GLfloat dir_z = sin(yaw);
    gluLookAt(aix_x, aix_y, aix_z, aix_x + dir_x, aix_y, aix_z + dir_z, 0, 1, 0); // Updated to use yaw

    //overally increasy the light levels
    GLfloat global_ambient[] = { 0.5f, 0.5f, 0.5f, 1.0f };
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, global_ambient);


    DisplayText(day);

    //usefull prints:
    //std::cout << "Camera Position: (" << "x: " << aix_x << ", " << "y: " << aix_y << ", " << "z: " << aix_z << "), Angle: " << Angle << std::endl;
    std::cout << "speed_controll: " << speed_controll << std::endl;

    get_sun();
    get_mercu();
    get_ven();
    get_earth();
    get_mars();
    get_jup();
    get_sat();
    get_ura();
    get_nep();
    
    glFlush();   
    glutSwapBuffers();
    
}

//Here the timing function is used to control the speed of frame rate
void timerProc(int id)
{
    static int elapsedTime = 0; // Variable to track elapsed time

    // Increment elapsedTime by the time since last frame
    elapsedTime += 17; // Add 17 milliseconds per frame

    // Check if a second has passed
    if (elapsedTime >= 1000 * speed_controll) {
        ++day;
        t_day++;
        std::cout << "Day count: " << day << std::endl;
        elapsedTime -= 1000 * speed_controll; // Reset elapsedTime after incrementing day
    }



    glutPostRedisplay();

    // Continue to call the timer function every 17 milliseconds for 60fps
    glutTimerFunc(17, timerProc, 1);
}

void mykeyboard(unsigned char key, int x, int y) {
    float movementSpeed = 100000.0f; // Adjust the speed as necessary
    float rotationSpeed = 0.05; // Rotation speed in radians

    switch (key) {
    case 'W':
    case 'w':
        aix_x += cos(yaw) * movementSpeed;  // Forward in the direction camera is facing
        aix_z += sin(yaw) * movementSpeed;
        break;
    case 'S':
    case 's':
        aix_x -= cos(yaw) * movementSpeed;  // Backward
        aix_z -= sin(yaw) * movementSpeed;
        break;
    case 'A':
    case 'a':
        aix_x -= sin(yaw) * movementSpeed;  // Strafe left
        aix_z += cos(yaw) * movementSpeed;
        break;
    case 'D':
    case 'd':
        aix_x += sin(yaw) * movementSpeed;  // Strafe right
        aix_z -= cos(yaw) * movementSpeed;
        break;
    case 'J':
    case 'j':
        yaw -= rotationSpeed;  // Rotate left
        break;
    case 'L':
    case 'l':
        yaw += rotationSpeed;  // Rotate right
        break;
    case 32:  // Spacebar for moving up
        aix_y += movementSpeed;
        break;
    case 'i':
    case 'I':  // Spacebar for moving up
        aix_y += movementSpeed;
        break;
    case 'k':
    case 'K':  // Spacebar for moving up
        aix_y -= movementSpeed;
        break;
    case 't':
    case 'T':  // Spacebar for moving up
        speed_controll += 0.10f;
        break;
    case 'y':
    case 'Y':  // Spacebar for moving up
        speed_controll -= 0.10f;
        break;
    }

    glutPostRedisplay();
}

void mouse(int button, int state, int x, int y) {
    if (button == 3 && state == GLUT_UP) { // Mouse wheel up
        cameraDistance -= 100000000; // Zoom in
    }
    else if (button == 4 && state == GLUT_UP) { // Mouse wheel down
        cameraDistance += 100000000; // Zoom out
    }
    glutPostRedisplay();
}

int main(int argc, char* argv[])

{

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
    glutInitWindowPosition(100, 100);
    glutInitWindowSize(Windowwidth, Windowheight);
    
    glutCreateWindow("Solar System");
    init_LoadallTexture();
    glutDisplayFunc(&myDisplay);
    glutKeyboardFunc(mykeyboard);
    glutMouseFunc(mouse);
    glutTimerFunc(50, timerProc, 1);
    glutMainLoop();

    return 0;
}