# 규칙
ROS2 Humble에서 사용 가능한 코드 작성하기.
Agent–User conversation: Use Korean (한국어)
코드, 주석, 커밋 메시지 : 영어
코드는 src 파일 안에 작성

# 요구사항
3D Lidar의 pointcloud2 토픽을 입력받아서(토픽 이름은 "ouster/points") 지면 제거 후 Spatio Temporal Voxel Layer (STVL) 참고하여 2d costmap을 발행한다.