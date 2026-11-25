#pragma once

/*!
 * \brief Идентификаторы для классов, использующих сокеты
 */
#define SOCKID_ALTIVAR 1000

 /*!
  * \class i_iot_node
  *
  * \brief Класс-интерфейс для связывания устройств в программе с реальными устройствами
  *
  */
class i_iot_node
	{
	public:
		virtual int set_state(int value, int index = 0);
		virtual int get_state(int index = 0);
		virtual int set_value(float value, int index = 0);
		virtual float get_value(int index = 0);
		virtual unsigned long set_counter(unsigned long value, int index = 0);
		virtual int get_counter(int index = 0);

		virtual ~i_iot_node() = default;
	};
