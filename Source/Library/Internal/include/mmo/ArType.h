#pragma once

// 기본 단위 타입 정의


// 요구사항 : float & double 로 형변환 가능해야하고 signed 여야함.
typedef	float				AR_UNIT;

// 요구사항 : 64비트 여야 함.
typedef	unsigned int		AR_HANDLE;

// is 1/100 second 
// 요구사항 : unsigned 이어야 함.
typedef	unsigned long		AR_TIME;

const AR_TIME INFINITE_TIME	= 0xffffffff;
