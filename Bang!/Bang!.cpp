#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include <time.h>
#include <malloc.h>
#define MAX_SIZE_GUN 6				//한 탄창 당 담을 수 있는 총 탄 수
#define MAX_HEALTH 5				//처음 체력(이거 담배 써도 넘으면 안되는건가)
#define MAX_ITEM 3					//최대 아이템 수

typedef struct bullet {
	int dmg;						//총알 데미지(공포탄이면 0, 실탄이면 1, 톱아이템으로 데미지 수정가능)
	bool doAIknow;					//ai구현용 상대가 공포탄인지 실탄인지 알고있는지 판단용
	struct bullet* link;
}Bullet;

typedef struct gun {
	int livenum;					//실탄 수
	int blanknum;					//공포탄 수
	int bulletnum;					//남은 총 탄 수(실탄+공포탄)
	Bullet* front;
	Bullet* rear;
}Gun;

typedef struct actor {
	char name[5];					//객체 이름. 플레이어 이름 입력용인데 구현 안했고 굳이?
	int HP;							//체력
	bool canActorMove;				//수갑 확인용
	//int Saw;						//톱 미구현 지워도 됨
	int Cigarette;					//담배
	//int Medicine;					//약 미구현 지워도 됨
	int Handcuffs;					//수갑
	int Dotbogi;					//Dotbogi
	int Money;						//소지금액
	int wincount;					//단계 도전용 숫자. 
}Actor;

void initGun(Gun* gun);				//게임 시작할때 총 객체 생성후 초기화용
int is_empty(Gun* gun);				//알제?
int is_full(Gun* gun);				//알제2?
Bullet createBullet();				//총알 노드 생성. 공포탄 실탄중 랜덤. ai가 공포탄인지 실탄인지 아는지 초기값은 당연히 FALSE, 돋보기 쓰면 TRUE로 바뀜
void enqueueGun(Gun* gun, Bullet bullet);	//총 객체에 총알 노드 추가
int dequeueGun(Gun* gun);					//총 쏠 때 총 객체에서 총알 노드 제거
int peakGun(Gun* gun);						//돋보기 사용용 함수
void print_Gun(Gun* gun, int temp);			//디버깅용 함수임 신경X
void initActor(Actor* actor);				//액터 초기화 함수. 플레이어 + 상대
void showInventory(Actor* actor);			//그냥 print함수 묶음임
Actor* Game(Actor* player, int ai);			//벅샷 진행 함수
Actor* Shop(Actor* actor);					//상점 함수
//정의로 이동으로 비교하면서 확인해보쇼

int main() {
	srand(time(NULL));									//랜덤 무작위로 알잘딱
	Actor* player= (Actor*)malloc(sizeof(Actor));		//플레이어 객체 생성
	initActor(player);									//초기화

	int select = 0;										//게임, 상점, 종료 선택용 변수
	//종료를 3에 넣어놔서 selcet가 3이 아닐때 반복되게 만듦
	while (select != 3) {
		printf("무엇을 할까?\n");
		printf("1. 상대에게 도전한다.\n");
		printf("2. 상점에 방문한다.\n");
		printf("3. 종료\n");
		scanf("%d", &select);
		if (select == 1) {
			printf("난이도를 선택하세용가리치킨\n1.쉬움 2. 보통 3.어려움\n");
			scanf("%d", &select);
			if (select == 1) {
				Game(player, 1);
			}
			else if (select == 2) {
				if (player->wincount < 1) {		//단계 도전용 변수 wincount ㅋㅋ
					printf("아직 이 상대에게 도전하지 못할 것 같다.\n");
					Sleep(500);
					system("cls");
					continue;
				}
				Game(player, 2);
			}
			else if (select == 3) {
				if (player->wincount < 2) {
					printf("아직 이 상대에게 도전하지 못할 것 같다.\n");
					Sleep(500);
					system("cls");
					select = 0;
					continue;
				}
				Game(player, 3);
				select = 0;
			}
			else {
				continue;
			}
		}
		else if (select == 2) {
			Shop(player);
		}
	}
	
	return 0;
}


void initGun(Gun* gun)			//연결리스트 큐 사용
{
	gun->front = NULL;
	gun->rear = NULL;
}

int is_empty(Gun* gun)
{
	return(gun->front == NULL);
}

int is_full(Gun* gun)
{
	return 0;//흠 full일 경우가 있나
}

Bullet createBullet()
{
	Bullet bullet;
	bullet.dmg = rand() % 2;
	bullet.doAIknow = FALSE;
	return bullet;
}

void enqueueGun(Gun* gun, Bullet bullet)		//성한울 교수님 연결리스트 수업자료 참고해서 만들었지비 1인지 2인진 기억 안난다
{
	Bullet* temp = (Bullet*)malloc(sizeof(Bullet));
	temp->dmg = bullet.dmg;
	temp->doAIknow = bullet.doAIknow;
	temp->link = NULL;
	if (is_empty(gun)) {
		gun->front = temp;
		gun->rear = temp;
	}
	else {
		gun->rear->link = temp;
		gun->rear = temp;
	}
}

int dequeueGun(Gun* gun)						//이것도
{
	Bullet* temp = gun->front;
	int dmg;
	if (is_empty(gun)) {
		exit(1);
	}
	else {
		dmg = temp->dmg;
		gun->front = gun->front->link;
		if (gun->front == NULL) gun->rear = NULL;
		free(temp);
		return dmg;
	}
}

int peakGun(Gun* gun)
{
	return gun->front->dmg;
}

void print_Gun(Gun* gun, int num)
{
	Bullet* temp = gun->front;
	for (int i = 0; i < num; i++) {
		printf("%d", gun->front->dmg);
		gun->front = gun->front->link;
	}
	printf("\n");
	gun->front = temp;
}

void initActor(Actor* actor)					//플레이어나 상대 초기화용. 기본적으로 주어지는 아이템 수나 체력은 여기서 변경 가능
{
	actor->canActorMove = TRUE;
	actor->Cigarette = 1;
	actor->Dotbogi = 1;
	actor->Handcuffs = 1;
	actor->HP = MAX_HEALTH;
	//actor->Medicine = 0;
	//actor->Saw = 0;
	actor->Money = 1000;
	actor->wincount = 0;
}

void showInventory(Actor* actor)				//print함수 묶음
{
	printf("현재 지닌 아이템 목록\n");
	printf("1. 담배 : %d개\n", actor->Cigarette);
	printf("2. 돋보기 : %d개\n", actor->Dotbogi);
	printf("3. 수갑 : %d개\n", actor->Handcuffs);
}

Actor* Game(Actor* player, int ai)
{
	Sleep(500);
	system("cls");
	Gun* gun = (Gun*)malloc(sizeof(Gun));			//게임 시작할때 총 생성, 게임 종료때 free 있음
	initGun(gun);
	int myhealth = player->HP;						//이게 약간 구시대 잔잰데 처음에 플레이어 객체없이 myhealth 선언해서 만들었다가 일일히 수정하기 귀찮아서 걍 대입으로 해결.
													//장점은 게임시작할때 체력이 최대체력 5로 초기화 된다는 점 why? player->HP는 변하지 않음 ㅋㅋ 그래서 return player해도 그대로 초기값 반환됨.

	Actor* enemy = (Actor*)malloc(sizeof(Actor));	//적 생성
	initActor(enemy);
	if (ai == 3) {									//난이도에 따라 적이 지니고 있는 아이템 수 변경용 기본은 다 1개씩
		enemy->Cigarette = 2;
		enemy->Dotbogi = 2;
		enemy->Handcuffs = 2;
	}
	else if (ai == 2) {

	}
	else {

	}
	int enemyhealth = enemy->HP;					//요것도 플레이어랑 동일
	printf("%d단계\n", ai);							//몇 단계인지 출력

	printf("게임 시작\n");
	printf("현재 나의 체력: %d 상대 체력: %d\n", myhealth, enemyhealth);
	int select = 0;									//선택용 변수 2개
	int enemyselect = 0;
	Sleep(1000);
	while ((myhealth > 0) && (enemyhealth > 0)) {	//이것도 무성이기획서 보다가 좀 꼬인부분인데 원래 while(myhealth && enemyhealth)였어서 중간에 체력관련 break문 존나 많은거임
		if (gun->rear == NULL) {					//총알 다 쐈을때&처음 시작할때 총 생성하려고 조건문
			printf("\n새 탄창!\n");
			Sleep(1000);
			int temp = (rand() % MAX_SIZE_GUN) + 1;
			int livesum = 0;
			for (int i = 0; i < temp; i++) {
				enqueueGun(gun, createBullet());
				livesum += gun->rear->dmg;
			}
			//print_Gun(gun, temp);
			gun->bulletnum = temp;
			gun->livenum = livesum;
			gun->blanknum = temp - livesum;
			printf("[남은 총알 수: %d발 실탄: %d발 공포탄: %d발]\n", gun->bulletnum, gun->livenum, gun->blanknum);
		}

		printf("나의 차례\n");
		while (player->canActorMove) {				//수갑 찬 상태면 실행안됨 뒤에 다시 TRUE로 바꾸는거 있음
			Sleep(500);
			printf("무엇을 할 것인가?\n1.격발 2.아이템\n");
			Sleep(500);
			scanf("%d", &select);
			if (select == 1) {
				printf("누굴 쏠 것 인가?\n1.나 자신 2. 상대\n");
				Sleep(500);
				scanf("%d", &select);
				if (select == 2) {
					printf("탕!...\n");
					Sleep(1000);
					if (dequeueGun(gun)) {
						printf("실탄이었다!\n");
						enemyhealth--;
						gun->livenum--;
					}
					else {
						printf("공포탄이었다...\n");
						gun->blanknum--;
					}
					break;
				}
				else {
					printf("탕!...\n");
					if (dequeueGun(gun)) {
						printf("윽! 실탄이었다..\n");
						myhealth--;
						gun->livenum--;
						break;
					}
					else {
						printf("휴! 공포탄이었다~\n");
						gun->blanknum--;
					}
				}
			}
			else if (select == 2) {
				printf("무슨 아이템을 사용할까?\n");	//아이템들은 다 가지고 있을때만 사용가능
				showInventory(player);
				scanf("%d", &select);
				if ((select == 1) && (player->Cigarette > 0)) {
					printf("뻐끔뻐끔\n");
					Sleep(1000);
					printf("체력을 회복했다.\n");
					myhealth++;
					player->Cigarette--;
				}
				else if ((select == 2) && (player->Dotbogi > 0)) {
					if (peakGun(gun) == 1) {
						printf("실탄이다.\n");
					}
					else {
						printf("공포탄이다.\n");
					}
					player->Dotbogi--;
				}
				else if ((select == 3) && (player->Handcuffs > 0)) {
					enemy->canActorMove = FALSE;
					printf("상대의 손에 수갑을 채웠다.\n");
					player->Handcuffs--;
				}
				else {
					printf("아이템을 가지고 있지 않는 듯 하다.");
				}
			}
			else if (select == 9) {
				enemyhealth = 0;
			}
			Sleep(500);
			gun->bulletnum = gun->livenum + gun->blanknum;
			printf("\n현재 나의 체력: %d 상대 체력: %d\n", myhealth, enemyhealth);
			printf("[남은 총알 수 : %d발]\n", gun->bulletnum);			//이거 남은 공포탄수하고 실탄수는 직접 센다길래 전체 남은 총알 수만 출력하는중
			if (enemyhealth == 0) break;								//여기가 꼬였다는 부분인데 나중에 천천히 고쳐봄
			if (myhealth == 0) break;
			if (gun->rear == NULL) break;								//이건 필요함 총알 다 쐈을 때 내 턴 무조건 종료되게
		}
		if (!player->canActorMove) {									//이게 수갑으로 못움직였을 때 다음턴 행동 가능하게 바꾸는 거
			printf("수갑을 풀어내느라 총을 쏘지 못했다.\n");
			player->canActorMove = TRUE;
		}
		if (enemyhealth <= 0) break;
		if (myhealth <= 0) break;
		if (gun->rear == NULL) continue;								//이것도 필요함 총알 다 쐈을 때 상대 턴 건너뛰고 바로 생성으로 넘어가기 위함

		printf("\n상대의 차례!\n");										//이건 뭐 if문 조건 잘 읽어보쇼
		while (enemy->canActorMove) {
			Sleep(1000);
			if (ai == 1) {
				enemyselect = 1;
			}
			else if (ai == 2) {
				if (gun->livenum >= gun->blanknum) {
					enemyselect = 1;
				}
				else {
					enemyselect = 2;
				}
			}
			else if (ai == 3) {
				if (gun->front->doAIknow) {
					if (gun->front->dmg) {
						enemyselect = 1;
					}
					else {
						enemyselect = 2;
					}
				}
				else if ((enemy->Dotbogi > 0) && (gun->livenum > 0) && (gun->blanknum>0)) {
					enemyselect = 5;
				}
				else if ((enemy->Handcuffs > 0) && (player->canActorMove == TRUE) && (gun->livenum>0)) {
					enemyselect = 4;
				}
				else if ((enemy->Cigarette > 0)&&(enemyhealth<MAX_HEALTH)) {
					enemyselect = 3;
				}
				else if (gun->livenum >= gun->blanknum) {
					enemyselect = 1;
				}
				else {
					enemyselect = 2;
				}
			}
			if (enemyselect == 1) {
				printf("상대는 이쪽으로 총구를 겨눴다.\n");
				Sleep(1000);
				printf("탕!...\n");
				if (dequeueGun(gun)) {
					printf("윽! 실탄이었다..\n");
					myhealth--;
					gun->livenum--;
					break;
				}
				else {
					printf("휴! 공포탄이었다~\n");
					gun->blanknum--;
					break;
				}

			}
			else if (enemyselect == 2) {
				printf("상대는 자신의 머리에 총구를 겨눴다.\n");
				Sleep(1000);
				printf("탕!...\n");
				if (dequeueGun(gun)) {
					printf("ㅋㅋ\n");
					enemyhealth--;
					gun->livenum--;
					break;
				}
				else {
					printf("이런 공포탄이었다...\n");
					gun->blanknum--;
				}
			}
			else if (enemyselect == 3) {
				printf("상대는 담배를 피기 시작했다.\n");
				enemyhealth++;
				enemy->Cigarette--;
			}
			else if (enemyselect == 4) {
				printf("상대는 내 손에 수갑을 채웠다.\n");
				Sleep(1000);
				player->canActorMove = FALSE;
				enemy->Handcuffs--;
			}
			else if (enemyselect == 5) {
				printf("상대는 돋보기로 총을 살피기 시작했다.\n");
				Sleep(1000);
				gun->front->doAIknow = TRUE;
				enemy->Dotbogi--;
			}
			Sleep(500);
			if (enemyhealth == 0) break;									//플레이어턴 쪽이랑 동일
			if (myhealth == 0) break;
			if (gun->rear == NULL) break;
		}
		if (!enemy->canActorMove) {
			printf("상대는 수갑을 풀어내느라 총을 쏘지 못했다.\n");
			enemy->canActorMove = TRUE;
		}
		gun->bulletnum = gun->livenum + gun->blanknum;
		printf("\n현재 나의 체력: %d 상대 체력: %d\n", myhealth, enemyhealth);
		printf("[남은 총알 수 : %d발]\n", gun->bulletnum);
		if (enemyhealth == 0) break;
		if (myhealth == 0) break;

	}
	free(gun);																//둘 중 아무나 체력 0되면 총 free

	if (myhealth > enemyhealth) {											//둘 중 아무나 체력 0되면 종료되는거라 체력 비교해서 승리 패배 결정. 어차피 한명은 0이고 한명은 0보다 큼
		printf("승리!\n");
		if (ai == 3) {
			player->Money += 10000;											//승리시 얻는 돈
		}
		else if (ai == 2) {
			player->Money += 5000;
			player->wincount = 2;
		}
		else {
			player->Money += 1000;
			player->wincount = 1;
		}
		Sleep(1000);
		system("cls");
	}
	else {
		printf("패배했다...\n");
		Sleep(1000);
		system("cls");
	}
	return player;
}

Actor* Shop(Actor* actor)													//상점 함수 별거 없음 그냥 player return하려고 Actor*형 함수로 선언한 것.
{
	Sleep(500);
	system("cls");
	int select = 0;
	while (select != 5){
		printf("현재 소지 금액: %d원\n", actor->Money);
		printf("\n");
		printf("1. 담배 : 500원\n");
		printf("2. 돋보기 : 1000원\n");
		printf("3. 수갑 : 1500원\n");
		printf("4. 현재 지닌 물건 확인하기.\n");
		printf("5. 구매 종료\n");
		scanf("%d", &select);
		switch (select) {
		case 1:
			if (actor->Money >= 500) {
				actor->Money -= 500;
				actor->Cigarette++;
				printf("담배를 구매했다.\n");
			}
			else {
				printf("돈이 부족하다...");
			}
			break;
		case 2:
			if (actor->Money >= 1000) {
				actor->Money -= 1000;
				actor->Dotbogi++;
				printf("돋보기를 구매했다.\n");
			}
			else {
				printf("돈이 부족하다...");
			}
			break;
		case 3:
			if (actor->Money >= 1500) {
				actor->Money -= 1500;
				actor->Handcuffs++;
				printf("돋보기를 구매했다.\n");
			}
			else {
				printf("돈이 부족하다...\n");
			}
			break;
		case 4:
			showInventory(actor);
			break;
		default: break;
		}
		Sleep(500);
		system("cls");
	}
	return actor;

}