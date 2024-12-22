/****************************************************************
 * File Function: ����������ľ���ʵ��
 * Author:        ����
 ****************************************************************/
#include "WeatherManager.h"
#include "../Constant/Constant.h"

USING_NS_CC;

// WeatherManager�ࣺ���캯��
WeatherManager::WeatherManager()
    : currentWeatherSystem(nullptr), backgroundMask(nullptr) {
}

// WeatherManager�ࣺ��������
WeatherManager::~WeatherManager() {
    removeWeatherEffect();
}

// WeatherManager�ࣺ��̬��������
WeatherManager* WeatherManager::create() {
    WeatherManager* weatherManger = new WeatherManager();
    if (weatherManger && weatherManger->init()) {
        weatherManger->autorelease();
        return weatherManger;
    }
    CC_SAFE_DELETE(weatherManger);
    return nullptr;
}

// ÿ�ո�������
void WeatherManager::updateWeather(Weather weather) {
    removeWeatherEffect();  // �Ƴ����е�����Ч��

    // �Ƴ����е���������ϵͳ
    if (currentWeatherSystem) {
        removeChild(currentWeatherSystem);
        currentWeatherSystem = nullptr;
    }

    // ��������ѡ��ͬ������ϵͳ
    switch (weather) {
    case Weather::Sunny:
        // ���죺����ʹ�ù��Ч����򵥵����챳��
        currentWeatherSystem = ParticleSun::create();
        currentWeatherSystem->setPosition(WINSIZE.width - SUN_OFFSET, WINSIZE.height - SUN_OFFSET); // ���Ͻ�
        currentWeatherSystem->setStartColor(Color4F(1.0f, 0.9f, 0.5f, 1.0f));	// ̫������ɫ
        currentWeatherSystem->setEndColor(Color4F(1.0f, 0.8f, 0.3f, 0.8f));		// ̫���������ɫ
        currentWeatherSystem->setSpeed(SUNNY_SET_SPEED);						// �޷�

        // ɾ����������
        if (backgroundMask) {
            removeChild(backgroundMask);
            backgroundMask = nullptr;
        }
        break;

    case Weather::LightRain:
        // С�꣺����С�������ϵͳ
        currentWeatherSystem = ParticleRain::create();
        currentWeatherSystem->setPosition(WINSIZE.width / 2, WINSIZE.height);	// ��Ļ����
        currentWeatherSystem->setStartColor(Color4F(0.4f, 0.4f, 1.0f, 1.0f));	// ǳ��ɫ���
        currentWeatherSystem->setEndColor(Color4F(0.2f, 0.2f, 0.8f, 0.8f));		// ��ν���
        currentWeatherSystem->setSpeed(LIGHTRAIN_SPEED);						// ����ٶ�
        currentWeatherSystem->setSpeedVar(LIGHTRAIN_SPEED_VAR);					// ����һЩ�ٶȵ������
        currentWeatherSystem->setStartSize(LIGHTRAIN_SIZE);						// ������εĴ�С
        currentWeatherSystem->setEndSize(LIGHTRAIN_SIZE_END);									// ��С

        // ��ӱ�������
        if (!backgroundMask) {
            backgroundMask = LayerColor::create(Color4B(0, 0, 0, 32));			// ��͸����ɫ
            addChild(backgroundMask);									        // ��ӵ��������㼶��������Ч��
        }
        break;

    case Weather::HeavyRain:
        // ���꣺���ø�����ٶȺ͸�������

        currentWeatherSystem = ParticleRain::create();
        currentWeatherSystem->setPosition(WINSIZE.width / 2, WINSIZE.height);	// ��Ļ����
        currentWeatherSystem->setStartColor(Color4F(0.4f, 0.4f, 1.0f, 1.0f));	// ��ɫ���
        currentWeatherSystem->setEndColor(Color4F(0.2f, 0.2f, 0.8f, 0.8f));		// ����
        currentWeatherSystem->setTotalParticles(600);							// �������ӵ���������������
        currentWeatherSystem->setSpeed(HEAVYRAIN_SPEED);						// �����ٶ�
        currentWeatherSystem->setSpeedVar(HEAVYRAIN_SPEED_VAR);					// �����ٶȵ������
        currentWeatherSystem->setStartSize(HEAVYRAIN_SIZE);						// ��������
        currentWeatherSystem->setEndSize(HEAVYRAIN_SIZE_END);					// ��С
        // ��ӱ�������
        if (!backgroundMask) {
            backgroundMask = LayerColor::create(Color4B(0, 0, 0, 64));			// ����İ�͸����ɫ
            addChild(backgroundMask);									        // ��ӵ��������㼶��������Ч��
        }
        break;

    case Weather::Snowy:
        // ѩ�죺ѩ��������ϵͳ
        currentWeatherSystem = ParticleSnow::create();
        currentWeatherSystem->setPosition(WINSIZE.width / 2, WINSIZE.height);	// ��Ļ����
        currentWeatherSystem->setStartColor(Color4F(1.0f, 1.0f, 1.0f, 1.0f));	// ��ɫѩ��
        currentWeatherSystem->setEndColor(Color4F(0.8f, 0.8f, 0.8f, 0.8f));		// ����Ϊ͸��
        currentWeatherSystem->setSpeed(SNOWY_SPEED);							// �����ѩ���ٶ�
        currentWeatherSystem->setSpeedVar(SNOWY_SPEED_VAR);						// ��΢�е������
        currentWeatherSystem->setStartSize(SNOWY_SIZE);							// �еȴ�Сѩ��
        currentWeatherSystem->setEndSize(SNOWY_SIZE_END);						// ��С
        break;
    }

    // �������ϵͳ������
    if (currentWeatherSystem) {
        addChild(currentWeatherSystem);
    }
}

// �Ƴ���ǰ����Ч��
void WeatherManager::removeWeatherEffect() {
    // �Ƴ���ǰ��������ϵͳ
    if (currentWeatherSystem) {
        removeChild(currentWeatherSystem);
        currentWeatherSystem = nullptr;
    }

    // �Ƴ���ǰ��������
    if (backgroundMask) {
        removeChild(backgroundMask);
        backgroundMask = nullptr;
    }
}