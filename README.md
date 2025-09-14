# ASCII-Graphics-Parallel-Optimization

<br>
<br>
<br>


병렬컴퓨팅을 이용해서

https://github.com/KimximyaFan/ASCII-Graphics-Implementation

여기서 구현했었던 ASCII 그래픽 성능을 끌어올려 본다

<br>
<br>
<br>
<br>
<br>
<br>

## 구현 점검

<br>
<br>

### 문제점 1

<img width="835" height="349" alt="image" src="https://github.com/user-attachments/assets/b39ca5ac-365f-4973-a545-2f45cde7e767" />

구현된 파이프라인을 도식화 해보면 위 그림과 같다

Render() 는 화면 한장을 그리기

DrawMesh() 는 매쉬의 버텍스에 각종 처리와 투영하고 삼각형 뽑아내기

Rasterize() 는 장면에 삼각형을 그리기이다

해당 과정은

엔티티 루프-> 엔티티에 속한 매쉬루프 -> 해당 매쉬에 속한 삼각형 루프 -> 레스터라이즈

를 거치는데, dependency가 엄청 크게 형성되어있다

여기다 나이브하게 멀티쓰레드를 박으면 분명 문제가 생길 것이다

<br>
<br>
<br>

### 문제점 2

<img width="236" height="300" alt="image" src="https://github.com/user-attachments/assets/618c3bbf-5950-4c37-b962-8bbc89367402" />

현재 Render()는 main.cc 에서 while 로 지속적으로 돌아간다

<br>
<br>
<br>

<img width="436" height="235" alt="image" src="https://github.com/user-attachments/assets/00633084-29a7-4ddd-8734-51f2cb169200" />

Render() 내부의 핵심로직은 위 부분이다

쓰레드 생성 종료시 OS를 거치면서 오버헤드가 꽤 크게 생기는데,

저 부분에 나이브하게 쓰레딩 돌려버리면

외부 while 루프로 인해서 엄청난 오버헤드가 생길 것이다

<br>
<br>
<br>

결론적으로, 최적화를 하기 위해서는

병렬화에 적합하게 파이프라인을 뜯어 고치고, 

오버헤드가 생기지 않도록 외부에서 쓰레드를 생성한 후, 해당 쓰레드를 지속적으로 재사용해야한다

<br>
<br>
<br>
<br>
<br>
<br>

## 쓰레드 풀

<img width="717" height="391" alt="image" src="https://github.com/user-attachments/assets/f1ee0565-2096-4f9b-a894-aa96b2ff0099" />

쓰레드 풀의 구조는 위 그림과 같다

몇 가지 구현 특징들을 집어보도록 한다

<br>
<br>
<br>

<img width="494" height="55" alt="image" src="https://github.com/user-attachments/assets/e21d5d94-c041-4cab-80d3-8976a9086d1f" />

쓰레드 풀 생성시, thread_count 만큼 쓰레드를 생성해서 vector에 넣으며,

각 쓰레드마다 WorkerLoop가 돌아간다

<br>
<br>
<br>

<img width="561" height="558" alt="image" src="https://github.com/user-attachments/assets/80238f6c-2643-4831-b8bb-a79d219dd307" />

worker loop 코드는 위 그림과 같다

<br>
<br>
<br>

<img width="339" height="37" alt="image" src="https://github.com/user-attachments/assets/457c2d4d-7751-4d69-954c-59e152f3378a" />

각 쓰레드들은 자기 블록에서 lock을 생성하지만 객체 변수인 mutex를 통해 경합을 한다

<br>
<br>
<br>

<img width="495" height="87" alt="image" src="https://github.com/user-attachments/assets/ea6d7df0-75dc-444a-aee5-6e0adca9ba08" />

쓰레드들은 일이 없으면 not_empty.wait()에 걸려있다

not_empty.wait(lock) 은

락을 잠깐 풀고 잠들었다가, 깨어날 때 다시 그 같은 락을 잡은 뒤 리턴하는 함수다

해당 wait()는 Start()를 통해 깨어나게 되며, 후 에 lock 경쟁을 한다

<br>
<br>
<br>

<img width="186" height="35" alt="image" src="https://github.com/user-attachments/assets/4755fc38-9498-41f5-b341-a1f70b82d971" />

락을 잡으면 GetTask()를 통해 실행 시켜야할 함수를 가져온다

<br>
<br>
<br>

<img width="340" height="136" alt="image" src="https://github.com/user-attachments/assets/dd73a749-a253-40ce-a775-aaac99ca0c15" />

Task를 가져오는건 큐를 조작하기 때문에, 동기화를 위해서 락이 필요하다

<br>
<br>
<br>

<img width="196" height="81" alt="image" src="https://github.com/user-attachments/assets/e8224072-285c-491e-ae1f-c858a99a0408" />

task를 가져왔으면 락을 풀고 할당된 함수를 실행한다

<br>
<br>
<br>

<img width="275" height="88" alt="image" src="https://github.com/user-attachments/assets/78d61bcb-4e4e-46f8-abfe-a306df356f96" />

그 외에도,

PushTask()를 통해 큐에 작업을 집어넣고,

Start() 를 통해 쓰레드를 깨우고, 

Wait()를 통해 메인쓰레드는 잠들어 있다가, 풀의 모든 작업이 끝나면 다시 진행하게 된다

<br>
<br>
<br>
<br>
<br>
<br>

## 파이프라인 수정하기

만든 쓰레드 풀에 맞춰서 파이프 라인을 수정해보자

<br>
<br>
<br>

<img width="835" height="349" alt="image" src="https://github.com/user-attachments/assets/b39ca5ac-365f-4973-a545-2f45cde7e767" />

현재 파이프라인 구현은 위와 같다

렌더링과정을 크게 둘로 나누면 매시드로잉과 레스터라이즈로 나눌 수 있는데,

메시에 적용되는 각종 처리들은 독립적이므로 병렬화가 가능하다

이미지배열도 잘게 나눠서 부분부분 그릴 수 있으므로 병렬화가 가능하다

그래서, 두 과정을 병렬화 가능하게 수정해야한다

<br>
<br>
<br>

<img width="956" height="515" alt="image" src="https://github.com/user-attachments/assets/fed8c3d5-6837-45a6-ac13-f8d78e10333d" />

파이프라인을 수정하면 위 그림처럼 될 것이다

<br>
<br>
<br>
<br>
<br>

### Draw Mesh 수정하기

매시드로잉이 하는일은 프레임버퍼(이미지)에 찍힐 삼각형을 구하는 것이다

그렇다면 삼각형만 따로 모아둘 수 있다

그런데 병렬로 처리할 것이므로 단순 배열을 쓰면 경합이 일어나 문제가 생긴다

<br>
<br>
<br>

<img width="646" height="599" alt="image" src="https://github.com/user-attachments/assets/c83eba19-122f-4e9a-a676-ae175cad69f0" />

중첩리스트와 쓰레드별 id를 사용해서,

해당 쓰레드는 해당 쓰레드id의 리스트만 사용함으로써 Lock-Free 하게 삼각형들을 집어넣을 수 있다

<br>
<br>
<br>

<img width="266" height="107" alt="image" src="https://github.com/user-attachments/assets/078f379a-02d7-4b68-a3a7-ad296921613b" />

Projected Triangle 구조체는 위 그림과 같다

삼각형의 세점을 의미하는 투영된 점 Projected Vertex와 해당 매쉬의 텍스쳐가 담긴다

후에 해당 삼각형은 tile단위 레스터라이즈에 쓰인다

<br>
<br>
<br>

<img width="443" height="31" alt="image" src="https://github.com/user-attachments/assets/e88fdb98-4be5-4867-bf0e-79287fd360dc" />

삼각형을 담을 자료구조는 위그림과 같다

<br>
<br>
<br>
<br>
<br>

### 타일

<img width="580" height="374" alt="image" src="https://github.com/user-attachments/assets/36c7ed16-5ab9-46c2-8256-c6ecbfdd317e" />

그림과 같은 이미지가 있다고 가정하자

<br>
<br>
<br>

<img width="563" height="356" alt="image" src="https://github.com/user-attachments/assets/06540191-497b-4e45-8a41-ac7bf419d3dc" />

이미지를 이렇게 잘게 타일로 나누고,

타일들을 Task화 시켜서 쓰레드풀에 넘길 예정이다

<br>
<br>
<br>

<img width="200" height="104" alt="image" src="https://github.com/user-attachments/assets/b6ea3c8a-cd2e-404a-a7e5-d8220083bf71" />

타일 구현은 위와 같다

각 타일에 해당하는 min, max 좌표와 id를 가지고 있다

<br>
<br>
<br>

<img width="467" height="447" alt="image" src="https://github.com/user-attachments/assets/63c030ed-5648-4cf3-a5eb-78bc28ef4e41" />

타일 만드는 구현은 위 그림과 같다

렌더러 객체 생성시 타일의 크기를 정해놓으면,

거기에 맞춰서 적절하게 잘라 타일 벡터에 집어넣는다

<br>
<br>
<br>
<br>
<br>


### 타일에 삼각형 할당하기

<img width="629" height="399" alt="image" src="https://github.com/user-attachments/assets/e09c970f-eadc-4204-9ed8-006a3b9d2118" />

그려야할 삼각형이 위 그림과 같다고 가정해보자

모든 타일이 해당 삼각형을 처리하는 것은 비효율적이므로, 

삼각형이 걸친 타일만 프로세스를 진행하도록 해야한다

그런데 정확히 삼각형 모양으로 걸친 타일을 계산하는건 구현이 까다롭고 연산이 많이 들어가므로

<br>
<br>
<br>

<img width="627" height="395" alt="image" src="https://github.com/user-attachments/assets/952af413-38d5-44eb-b2bb-d7057be4054b" />

훨씬 쉽고 직관적인 삼각형 바운딩 박스를 이용한다

박스가 걸쳐있으면 해당되는 타일에 삼각형이 할당된다

그런데, 레스터라이즈 할 때 타일 전체를 루프하는건 비효율적이다

<br>
<br>
<br>

<img width="631" height="400" alt="image" src="https://github.com/user-attachments/assets/c671a33e-406c-41b5-b7f4-e8758fc68585" />

위 그림의 tile 0 과 tile 1 을 보면, 

타일과 바운딩박스가 겹친 부분은 하늘색으로 영역이 그려져 있는데

겹친 영역의 만큼만 루프를 돌면 연산량을 줄일 수 있다

<br>
<br>
<br>

<img width="201" height="102" alt="image" src="https://github.com/user-attachments/assets/bb5a50bc-2a85-4449-92e5-a9dc9245b215" />

각 타일들이 이용할건 삼각형 레퍼런스이다

참조 인덱스와, 겹친 범위가 담긴다

<br>
<br>
<br>

<img width="434" height="33" alt="image" src="https://github.com/user-attachments/assets/10127445-56e3-490a-b244-534a4474c71e" />

참조 인덱스는 메쉬드로잉에서 처리해뒀던 삼각형을 담았던 triangles를 참조한다

<br>
<br>
<br>

<img width="439" height="30" alt="image" src="https://github.com/user-attachments/assets/0e4fb83f-a2b9-44d8-a41d-b256fe8d7a9e" />

삼각형 레퍼런스는 여기에 담긴다

각 타일들은 자기 id가 존재하므로

id 별 벡터에 삼각형레퍼런스가 담긴다 

<br>
<br>
<br>

<img width="653" height="632" alt="image" src="https://github.com/user-attachments/assets/253aa841-a216-4793-8087-f4d035053932" />

삼각형을 타일에 할당하는 구현은 위 그림과 같다

삼각형의 바운딩박스를 구하고, 

박스 좌표를 타일 표준 width height로 나눠서, 겹친 타일들의 가로세로 인덱스 범위를 구하고

for문을 돌면서 타일별로 참조인덱스와, 공통 범위를 담는 레퍼런스를 넣는다

<br>
<br>
<br>
<br>
<br>


### 레스터라이즈

<img width="737" height="326" alt="image" src="https://github.com/user-attachments/assets/a80286fc-23df-48f8-8a70-36d3b297c217" />

쓰레드가 하게될 일은 위 그림과 같다

타일 하나가 Task가 되어서, 

쓰레드 별로 tile 1개를 붙잡고, 

해당 타일에 할당된 삼각형을 그리는 것이다

<br>
<br>
<br>

<img width="528" height="155" alt="image" src="https://github.com/user-attachments/assets/278f4a90-e51a-43c8-95c4-e322adb5c933" />

for문을 돌면서,

참조를 이용해서 삼각형을 뽑아낸다

이후 과정은 기존의 파이프라인과 동일하나

<br>
<br>
<br>

<img width="301" height="124" alt="image" src="https://github.com/user-attachments/assets/9edffc00-635c-4a05-a19f-5016daf47493" />

위에서 설명했던 것처럼, rasterize 할 때 교집합 영역만 포문을 돌아서 연산량이 줄어든다

<br>
<br>
<br>
<br>
<br>
<br>

## 성능 평가

성능은 FPS를 기준으로 한다

FPS를 구현하는 방법은 여러가지가 있다

그중에 최근 1초간 그려진 프레임수를 나타내는 방법이 있는데

<br>
<br>
<br>

![1](https://github.com/user-attachments/assets/3c5345b6-34e8-42ec-b59d-53adeb9616c0)

해당 방법은 값이 너무 튀어서 평가하기가 너무 까다롭다

그래서,

(그려진 프레임수) / 경과시간

을 이용해서 평균 FPS를 지표로 이용하기로 한다

같은 시간내에 더 많은 프레임을 그렸다면 성능이 더 좋다고 볼 수 있다

<br>
<br>
<br>

<img width="366" height="131" alt="image" src="https://github.com/user-attachments/assets/0c647416-a764-4c13-b3bc-31aba6a17381" />

평균 FPS 구현은 위와 같다

해당 함수는 매 while loop Render() 마다 실행된다

<br>
<br>
<br>

![2](https://github.com/user-attachments/assets/be4e7fbd-d15f-4f25-9d3b-de468fea416c)

싱글 쓰레드

<br>
<br>
<br>


![3](https://github.com/user-attachments/assets/038f59bc-e590-43fe-b5ea-13f936ec4745)

쓰레드 2개

30~40% 정도의 성능 향상이 있다고 볼 수 있다
