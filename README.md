# ASCII-Graphics-Parallel-Optimization




병렬컴퓨팅을 이용해서

https://github.com/KimximyaFan/ASCII-Graphics-Implementation

여기서 구현했었던 ASCII 그래픽 엔진의 성능을 끌어올려 본다



본론


구현 점검

문제점 1

<img width="835" height="349" alt="image" src="https://github.com/user-attachments/assets/b39ca5ac-365f-4973-a545-2f45cde7e767" />

구현된 파이프라인을 도식화 해보면 위 그림과 같다

Render() 는 화면 한장을 그리는 것

DrawMesh() 는 장면에 매쉬를 그리는 것

Rasterize() 는 장면에 삼각형을 그리는 것

엔티티 루프-> 엔티티에 속한 매쉬루프 -> 해당 매쉬에 속한 삼각형 루프 -> 레스터라이즈

구조라서 해당 파이프라인은 dependency가 엄청 크게 형성되어있다

여기다 나이브하게 멀티쓰레드를 박으면 분명히 문제가 생길 것이다




문제점 2

<img width="236" height="300" alt="image" src="https://github.com/user-attachments/assets/618c3bbf-5950-4c37-b962-8bbc89367402" />

현재 Render()는 main.cc 에서 while 루프로 지속적으로 돌아간다


<img width="436" height="235" alt="image" src="https://github.com/user-attachments/assets/00633084-29a7-4ddd-8734-51f2cb169200" />

Render() 내부의 핵심로직은 그림과 같다

저기에 나이브하게 쓰레딩 돌려버리면

위에 서술했던 외부 while 루프로 인해 엄청난 쓰레드 생성종료 오버헤드가 생길 것이다



종합해보면

병렬화에 적합하게 파이프라인을 뜯어 고치고, 

오버헤드가 생기지 않도록 외부에서 쓰레드를 생성한 후, 해당 쓰레드를 지속적으로 재사용해야한다




쓰레드 풀

<img width="717" height="391" alt="image" src="https://github.com/user-attachments/assets/f1ee0565-2096-4f9b-a894-aa96b2ff0099" />

쓰레드 풀의 구조는 위 그림과 같다

몇 가지 간단한 구현 특징들을 집어보도록 한다

<img width="494" height="55" alt="image" src="https://github.com/user-attachments/assets/e21d5d94-c041-4cab-80d3-8976a9086d1f" />

쓰레드 풀 생성시, 미리 설정한 쓰레드 수만큼 쓰레드를 생성해서 vector에 넣고, 각 쓰레드마다 WorkerLoop가 돌아간다


