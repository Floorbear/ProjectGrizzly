// Fill out your copyright notice in the Description page of Project Settings.


#include "CPP_GrizzlyGameState.h"
#include "ProjectGrizzly/ProjectGrizzly.h"

//���Ӹ�尡 StartPlay�� �����Ҷ� ���ӽ�����Ʈ�� ����� ������ ���ӽ�����Ʈ�� ���忡 �ִ� �����Ϳ��� BeginPlay�� ȣ���ϵ��� ����� ������
//�� ���������� ����Ǵ� �����̱� ������ Ŭ���̾�Ʈ���� ȣ����� �ʴ´�
void ACPP_GrizzlyGameState::HandleBeginPlay()
{
	Super::HandleBeginPlay();

}

// HandleBeginPlay�� ������ ������ ����������
// bReplicatedHasBegunPlay ������Ƽ�� ����� Ŭ���̾�Ʈ�� �������� �� 'Ŭ���̾�Ʈ'������ ȣ��ȴ�
void ACPP_GrizzlyGameState::OnRep_ReplicatedHasBegunPlay()
{
	Super::OnRep_ReplicatedHasBegunPlay();

}
