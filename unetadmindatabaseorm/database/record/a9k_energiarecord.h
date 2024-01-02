#ifndef A9K_ENERGIARECORD_H
#define A9K_ENERGIARECORD_H

#include "qant3_global.h"
#include "recordbase.h"
#include "a9k_equiporecord.h"

class QANT3MVSHARED_EXPORT a9k_energiaRecord: public QAnt::Dao::RecordBase
{
	Q_OBJECT

protected:

	void initValues();

public:
	a9k_energiaRecord(QObject *qparent=0);
	a9k_energiaRecord(const a9k_energiaRecord &other);
	~a9k_energiaRecord();

	void set_a9k_equipo_id(int a9k_equipo_id);
	void set_bandeja0fuentes(int bandeja0fuentes);
	void set_bandeja1fuentes(int bandeja1fuentes);
	void set_cantidad_caida_fuentes_soportado(int cantidad_caida_fuentes_soportado);
	void set_capacidad_total(int capacidad_total);
	void set_cisco_caida_mitad_potencia_no_soportado(int cisco_caida_mitad_potencia_no_soportado);
	void set_cisco_capacidad_no_cableada(int cisco_capacidad_no_cableada);
	void set_date(QDate date);
	void set_disponible(int disponible);
	void set_id(int id);
	void set_nmas1(int nmas1);
	void set_no_soporta_caida_de_mas_de_una_fuente(int no_soporta_caida_de_mas_de_una_fuente);
	void set_total_requerido(int total_requerido);

	int a9k_equipo_id();
	int bandeja0fuentes();
	int bandeja1fuentes();
	int cantidad_caida_fuentes_soportado();
	int capacidad_total();
	int cisco_caida_mitad_potencia_no_soportado();
	int cisco_capacidad_no_cableada();
	QDate date();
	int disponible();
	int id();
	int nmas1();
	int no_soporta_caida_de_mas_de_una_fuente();
	int total_requerido();

	a9k_equipoRecord *r_a9k_equipo_id();

	a9k_energiaRecord& operator=(const a9k_energiaRecord &record);
};

Q_DECLARE_METATYPE(a9k_energiaRecord*)

#endif
