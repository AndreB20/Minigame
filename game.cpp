#include <iostream>
#include <string>
#include <windows.h>
#include <conio.h>

#define PI 3.141592

#include "Framework.h"

#define COL 5
#define ROW 3

class Object
{
public:
	const char* file;
	int w, h, x, y;
};

int WIDTH, HEIGHT;

/* Test Framework realization */
class MyFramework : public Framework 
{
public:
	Object ball;
	Object paddle;
	Object turret;
	Object star;
	Object bad;
	Object brick[ROW][COL];
	Sprite* paddleSprite;
	Sprite* ballSprite;
	Sprite* turretSprite;
	Sprite* starSprite;
	Sprite* badSprite;
	Sprite* brickSprite[ROW][COL];



	int SPEED = 9, BALL_SPEED = 7, SIZE, SPACING = 20;
	float velY, velX, turretVelX, turretVelY, starVel, badVel;

	int check, TURRET_SPEED = 4, ABILITY_SPEED = 3;
	bool bricks[ROW][COL], starStart, badStart, start = 0;
	int timerFPS, lastFrame, timeGame, turretTime = 0,
		starTime = 0, abilitySpawnXg = 0, abilitySpawnYg = 0,
		badTime = 0, abilitySpawnXb = 0, abilitySpawnYb = 0,
		redBrickX1 = 0, redBrickY1 = 0, redBrickX2 = 0, redBrickY2 = 0;
	int brickW = WIDTH / COL, brickH = HEIGHT / 4 / ROW;
	int starSaveW, starSaveH, badSaveW, badSaveH, paddleSaveW;
	bool LEFTon = 0, RIGHT = 0, LEFT = 0, UP = 0;

	//COLLISION
	bool intersect(int min0, int max0, int min1, int max1)
	{
		return max(min0, max0) >= min(min1, max1) &&
			   min(min0, max0) <= max(min1, max1);
	}
	bool collision(Object &obj0, Object &obj1)
	{
		return intersect(obj0.x, obj0.x + obj0.w, obj1.x, obj1.x + obj1.w) &&
			   intersect(obj0.y, obj0.y + obj0.h, obj1.y, obj1.y + obj1.h);
	}




	//Random launch
	int randomFuncTur(int theTick)			//Random function for in a 10 sec range from "getTickCount()"
	{
		theTick = theTick % 10000 / 2;
		
		return theTick;
	}


	//Random bricks (Red and for abilities) a little bit hardcoded (a little bit more)
	int m[2][4];
	void randomFuncBloc()
	{
		srand(time(0));
		int coord;
		for (int i = 0; i < 2; i++)
			for(int j = 0; j < 4; j++)
			{
				coord = rand();
				//generate
				if (i == 0)
					m[i][j] = coord % COL;
				if (i == 1 )
					m[i][j] = coord % ROW;

				if (checkRand())
					i = 0;
			}

		abilitySpawnXg = m[0][0]; abilitySpawnXb = m[0][1]; redBrickX1 = m[0][2]; redBrickX2 = m[0][3];
		abilitySpawnYg = m[1][0]; abilitySpawnYb = m[1][1]; redBrickY1 = m[1][2]; redBrickY2 = m[1][3];
	}
	bool checkRand()
	{
		int i = 0,x,y;

		for (int j = 0; j < 4; j++)
		{
			x = m[i][j];
			y = m[i + 1][j];
			for (int k = 0; k < 4; k++)
			{
				if(k!=j)
					if ((x == m[i][k]) && (y == m[i + 1][k]))
						return true;
			}
		}
		return false;
	}





	//Reset at 0 everything
	void resetBricks() 
	{
		for (int i = 0; i < ROW; i++)
			for (int j = 0; j < COL; j++)
				bricks[i][j] = 1;
	

		paddle.x = (WIDTH / 2) - (paddle.w / 2);			//Paddle reset in center screen
		paddle.y = HEIGHT - (HEIGHT / 10);

		paddle.w = paddleSaveW;

		ball.x = paddle.x + (paddle.w / 2) + SIZE;			//Ball with paddle
		ball.y = paddle.y - paddle.h;

		velY = 0;											//Before starting (in Update it changes)
		velX = 0;

		turret.x = WIDTH / 2;		//TURRET POS
		turret.y = 0 ;

		turretVelX = TURRET_SPEED;
		turretVelY = 0;  //TURRET SPEED BULLET

		starVel = ABILITY_SPEED;							//Good ability Vel that is SPEED variable for abilities
		starStart = 0;
		starTime = 0;
		star.w = starSaveW;
		star.h = starSaveH;

		badVel = ABILITY_SPEED;								//Bad ability Vel that is SPEED variable for abilities
		badStart = 0;	
		badTime = 0;
		bad.w = badSaveW;
		bad.h = badSaveH;

		start = 0;
	}



	//Position according to WIDTH,HEIGHT of screen and first brick created
	void setBricks(int i, int j)
	{
		brick[i][j].x = j * brick[0][0].w ;
		brick[i][j].y = i * brick[0][0].h ;
	}

	



	/////////////////////////////////////////////////////////// GAME LOGIC !
	void update()
	{
		//BALL COLLISION and LOGIC

		if (collision(ball, paddle) && start)					//PADDLE and BALL interaction
		{
			if (!LEFTon)
			{
				float rel = (paddle.x + (paddle.w / 2)) - (ball.x + (SIZE / 2));
				float norm = rel / (paddle.w / 2);
				float bounce = norm * (4 * PI / 12);
				velY = -BALL_SPEED * cos(bounce);
				velX = BALL_SPEED * -sin(bounce);

				if (velX >= -1 && velX <= 1)
				{
					if (velX >= 0)
						velX = 1;
					if (velX <= 0)
						velX = -1;
				}
			}			

			else
			{
				velX = 0;
				velY = 0;
				if (RIGHT)
				{
						ball.x = ball.x + BALL_SPEED;
				}

				if (LEFT)
				{
						ball.x = ball.x - BALL_SPEED;
				}
			}

		}


		
		if (collision(turret, paddle))									//Turret collision and logic
			resetBricks();



		if (turretTime >= randomFuncTur(lastFrame))						//Random shoots
		{
			turretVelX = 0;
			turret.y = turret.y + turretVelY;
		}

		if(turret.x >= WIDTH - (WIDTH / 6))
			turretVelX = -turretVelX;
		if (turret.x <= WIDTH / 6)
			turretVelX = -turretVelX;

		turret.x += turretVelX;											

		if (turret.y >= HEIGHT)											//Turret end and reset
		{
			turretTime = 0;
			turret.y = 0;
			turretVelX = TURRET_SPEED;
		}

		

		//GOOD ABILITY !
		//Good ability init
		if (bricks[abilitySpawnYg][abilitySpawnXg] == 1)
		{
			star.x = brick[abilitySpawnYg][abilitySpawnXg].x + brick[abilitySpawnYg][abilitySpawnXg].w / 2 - star.w;
			star.y = brick[abilitySpawnYg][abilitySpawnXg].y;
		}

		//BAD ABILITY !
		//Bad ability init
		if (bricks[abilitySpawnYb][abilitySpawnXb] == 1)
		{
			bad.x = brick[abilitySpawnYb][abilitySpawnXb].x + brick[abilitySpawnYb][abilitySpawnXb].w / 2 - bad.w;
			bad.y = brick[abilitySpawnYb][abilitySpawnXb].y;
		}

		//Ball screen collision
		if (ball.y <= 1) 
			velY = -velY;

		if (ball.x <= 1 || ball.x + SIZE >= WIDTH - 1)
			velX = -velX;


		//Ball and Brick collision/////////////////////////////////////////////////////////////////////////
		bool reset = 1;
		for (int i = 0; i < ROW; i++)
			for (int j = 0; j < COL; j++)
			{
				setBricks(i,j);											
				if (collision(ball, brick[i][j]) && bricks[i][j])		
				{
					check = 0;//Counts the destroyed bricks
					for (int k = 0; k < ROW; k++)
						for (int l = 0; l < COL; l++)
							if (bricks[k][l] == 0)
								check++;

					if ((i == redBrickY1 && j == redBrickX1) || (i == redBrickY2 && j == redBrickX2))		//Check the red brick in matrix Y is ROW and X for COL
					{
						if (check >= ROW * COL - 2)						//Check if all the bricks are gone and remains only the red ones for turning off
							bricks[i][j] = 0;							


						//Collision react
						if (ball.y >= brick[i][j].y || ball.y <= brick[i][j].y + brick[i][j].h - 10)//Y coord
						{
							velY = -velY;
						}
						else
						{
							velX = -velX;
						}

					}
					
					else
					{
						bricks[i][j] = 0;
						
						//Collision react
						if (ball.y >= brick[i][j].y  ||  ball.y  <= brick[i][j].y + brick[i][j].h - 10)//Y coord
						{
							velY = -velY;
						}
						else
						{
							velX = -velX;
						}	

					}
									
				}
				if (bricks[i][j])reset = 0;
			}


		//The good ability falls
		if (bricks[abilitySpawnYg][abilitySpawnXg] == 0)
		{
			star.y = star.y + starVel;
		}

		//The bad ability falls
		if (bricks[abilitySpawnYb][abilitySpawnXb] == 0)
		{
			bad.y = bad.y + badVel;
		}

		
		{//Collision with good ability
			
			if (collision(star, paddle))
			{
				paddle.w = paddle.w * 2;
				star.w = 0;
				star.h = 0;
				star.x = 0;
				star.y = 0;
				starVel = 0;
				starStart = 1;
			}

			if (starStart)
			{
				starTime = starTime + timeGame;

				if (starTime >= 10000)
				{
					starStart = 0;
					paddle.w = paddle.w / 2;
				}
			}
		}

		{//Collision with bad ability

			if (collision(bad, paddle))
			{
				paddle.w = paddle.w * 0.6;
				bad.w = 0;
				bad.h = 0;
				bad.x = 0;
				bad.y = 0;
				badVel = 0;
				badStart = 1;
			}

			if (badStart)
			{
				badTime = badTime + timeGame;

				if (badTime >= 10000)
				{
					badStart = 0;
					paddle.w = paddle.w / 0.6;
				}
			}
		}

		

		if (reset && collision(ball, paddle))
		{
			resetBricks();
			
		}
		
		if ((ball.y + ball.h >= HEIGHT))
		{
			resetBricks();
			
		}

		//Ball movement
		ball.y += velY;
		ball.x += velX;
	}




	virtual void PreInit(int& width, int& height, bool& fullscreen)
	{
		width = WIDTH;
		height = HEIGHT;
		fullscreen = false;
	}

	virtual bool Init()
	{
		//Reset is also the initialization
		randomFuncBloc();
		float screenRatio = (float)WIDTH / (float)HEIGHT;

		if (screenRatio >= 2 / 1)
			screenRatio -= 1;
		if (screenRatio <= 1 / 2)
			screenRatio += 1;
		

		//Paddle
		paddle.file = "./data/50-Breakout-Tiles.png";
		paddleSprite = createSprite(paddle.file);
		getSpriteSize(paddleSprite, paddle.w, paddle.h);
		float paddleRatio = (float)paddle.w / (float)paddle.h;
																			
		paddle.w = (float)paddle.w * (screenRatio)/ 6;
		paddle.h = (float)paddle.w / (float)paddleRatio;	//SIZE

		paddleSaveW = paddle.w;


		//Ball
		ball.file   = "./data/63-Breakout-Tiles.png";
		ballSprite = createSprite(ball.file);
		getSpriteSize(ballSprite, ball.w, ball.h);
		float ballratio = (float)ball.w / (float)ball.h;

		SIZE = ball.w * screenRatio / 2;					//SIZE
		ball.h = ball.w = SIZE;							
		

		//Turret
		turret.file = "./data/61-Breakout-Tiles.png";	
		turretSprite = createSprite(turret.file);
		getSpriteSize(turretSprite, turret.w, turret.h);
		float turretRatio = (float)turret.w / (float)turret.h;

		turret.h = float(turret.h) * screenRatio;			//SIZE						
		turret.w = turret.h * turretRatio;


		//Good ability
		star.file = "./data/59-Breakout-Tiles.png";	
		starSprite = createSprite(star.file);
		getSpriteSize(starSprite, star.w, star.h);
		float starRatio = (float)star.w / (float)star.h;

		star.w = (float)star.w * screenRatio / 5;
		star.h = star.w;									//SIZE


		//Bad ability
		bad.file = "./data/24-Breakout-Tiles.png";
		badSprite = createSprite(bad.file);
		getSpriteSize(badSprite, bad.w, bad.h);
		float badRatio = (float)bad.w / (float)bad.h;

		bad.w = (float)bad.w * screenRatio / 5;				//SIZE
		bad.h = bad.w;

		starSaveW = star.w; starSaveH = star.h; badSaveW = bad.w; badSaveH = bad.h;

		resetBricks();


		//Initialize everything besides brick
		setSpriteSize(paddleSprite, paddle.w, paddle.h);

		setSpriteSize(ballSprite, ball.w, ball.h);

		setSpriteSize(turretSprite, turret.w, turret.h);

		setSpriteSize(starSprite, star.w, star.h);
		
		setSpriteSize(badSprite, bad.w, bad.h);

		//BRICK INIT
		brick[0][0].file = "./data/01-Breakout-Tiles.png";
		brickSprite[0][0] = createSprite(brick[0][0].file);
		getSpriteSize(brickSprite[0][0], brick[0][0].w, brick[0][0].h);
		float BrickRatio = (float)brick[0][0].w / (float)brick[0][0].h;

		brick[0][0].w = WIDTH / COL;
		brick[0][0].h = brick[0][0].w / BrickRatio;
		
		for (int i = 0; i < ROW; i++)
			for (int j = 0; j < COL; j++)
			{
				brick[i][j].file = "./data/01-Breakout-Tiles.png";						//Blue
				if((i == redBrickY1 && j == redBrickX1) || (i == redBrickY2 && j == redBrickX2))
					brick[i][j].file = "./data/07-Breakout-Tiles.png";		//Red bricks

				if(i == abilitySpawnYg && j == abilitySpawnXg)
					brick[i][j].file = "./data/13-Breakout-Tiles.png";		//Good ability brick	

				if(i == abilitySpawnYb && j == abilitySpawnXb)
					brick[i][j].file = "./data/09-Breakout-Tiles.png";		//Bad ability brick


				brick[i][j].w = brick[0][0].w;	//Brick size
				brick[i][j].h = brick[0][0].h;

				setBricks(i, j);
				brickSprite[i][j] = createSprite(brick[i][j].file);
				setSpriteSize(brickSprite[i][j], brick[0][0].w, brick[0][0].h);
			}

	

		return true;
	}

	virtual void Close()
	{
		destroySprite(paddleSprite);
		destroySprite(ballSprite);
		destroySprite(turretSprite);
		destroySprite(starSprite);
		destroySprite(badSprite);
		for (int i = 0; i < ROW; i++)
			for (int j = 0; j < COL; j++)
				destroySprite(brickSprite[i][j]);
	}

	virtual bool Tick() 
	{
		lastFrame = getTickCount();
		
		update();

		timerFPS = getTickCount() - lastFrame;
		if (timerFPS < (1000/75)); 
		{
			timeGame = (1000 / 75) - timerFPS;
			Sleep(timeGame);
			turretTime = turretTime + timeGame;
		}

		if (RIGHT)
		{
			paddle.x = paddle.x + BALL_SPEED;
			if (start == 0)
				ball.x = ball.x + BALL_SPEED;
		}

		if (LEFT)
		{
			paddle.x = paddle.x - BALL_SPEED;
			if(start == 0)
				ball.x = ball.x -BALL_SPEED;
		}

		//std::cout << starTime << std::endl;
		
		drawSprite(paddleSprite, paddle.x, paddle.y);
		setSpriteSize(paddleSprite, paddle.w, paddle.h);	//Paddle modifying after abilities

		drawSprite(ballSprite, ball.x, ball.y);
		drawSprite(turretSprite, turret.x, turret.y);

		drawSprite(starSprite, star.x, star.y);
		setSpriteSize(starSprite, star.w, star.h);

		drawSprite(badSprite, bad.x, bad.y);
		setSpriteSize(badSprite, bad.w, bad.h);

		//Draw bricks
		for (int i = 0; i < ROW; i++)
			for(int j = 0; j < COL; j++)
			{
				if (bricks[i][j])
				{
					setBricks(i,j);
					drawSprite(brickSprite[i][j], brick[i][j].x, brick[i][j].y);
				}
			}


		return false;
	}

	virtual void onMouseMove(int x, int y, int xrelative, int yrelative) 
	{
		
	}

	virtual void onMouseButtonClick(FRMouseButton button, bool isReleased)	
	{
		if (button == FRMouseButton::LEFT && isReleased == false)
		{
			LEFTon = 1;
		}
		else
			LEFTon = 0;

		std::cout << LEFTon << " " << std::endl;
	}

	virtual void onKeyPressed(FRKey k) 
	{

		if (k == FRKey::RIGHT)
			RIGHT = 1;
		if (k == FRKey::LEFT)
			LEFT = 1;

		if (k == FRKey::UP)							//START GAME
		{
			velY = BALL_SPEED;
			turretVelY = TURRET_SPEED;
			start = 1;
		}

	}

	virtual void onKeyReleased(FRKey k)
	{
		if (k == FRKey::RIGHT)
			RIGHT = 0;
		if (k == FRKey::LEFT)
			LEFT = 0;
	}
	
	virtual const char* GetTitle() override { return "Arcanoid"; }
};


//Take Window WIDTHxHEIGHT and applies it
void createWindow()
{
	std::cout << "Input screen with command: game -window WIDTHxHEIGHT" << "\n";
	int x, y;
	std::string s;
	bool isCorrect = 1;

	while (isCorrect)
	{
	std::getline(std::cin, s);
		if (s.find("game -window") == 0)
		{
			std::string checkWin = s.substr(13, s.length());
			std::string w, h;
			int i, j;

			for (i = 0; i < checkWin.length(); i++)
			{
				if (checkWin[i] == 'x')
					break;
				if (checkWin[i] != 'x')
					w = w + checkWin[i];
			}

			for (j = i + 1; j < checkWin.length(); j++)
			{
				h = h + checkWin[j];
			}

			WIDTH = std::stoi(w);
			HEIGHT = std::stoi(h);
			isCorrect = 0;
		}
		else if (s.find("exit") == 0)
		{
			exit(0);
		}

		else
		{
			std::cout << "Please, write again:";
			isCorrect = 1;
		}
	}

}

int main(int argc, char *argv[])
{
	createWindow();
	return run(new MyFramework);
}
