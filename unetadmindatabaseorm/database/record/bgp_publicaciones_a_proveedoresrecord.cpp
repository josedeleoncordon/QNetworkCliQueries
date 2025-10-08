#include "bgp_publicaciones_a_proveedoresrecord.h"

bgp_publicaciones_a_proveedoresRecord::bgp_publicaciones_a_proveedoresRecord(QObject *qparent):
	RecordBase(nullptr,qparent)
{
	TABLENAME="bgp_publicaciones_a_proveedores";
}

bgp_publicaciones_a_proveedoresRecord::bgp_publicaciones_a_proveedoresRecord(const bgp_publicaciones_a_proveedoresRecord &other):
	RecordBase(other.RPARENT,other.parent())
{
	TABLENAME="bgp_publicaciones_a_proveedores";
	clone(other);
}

bgp_publicaciones_a_proveedoresRecord::~bgp_publicaciones_a_proveedoresRecord() {}

void bgp_publicaciones_a_proveedoresRecord::set_asn(QString asn)
{
	setProperty("asn",asn);
}

void bgp_publicaciones_a_proveedoresRecord::set_comunidad_base(QString comunidad_base)
{
	setProperty("comunidad_base",comunidad_base);
}

void bgp_publicaciones_a_proveedoresRecord::set_grupo(int grupo)
{
	setProperty("grupo",grupo);
}

void bgp_publicaciones_a_proveedoresRecord::set_interface(QString interface)
{
	setProperty("interface",interface);
}

void bgp_publicaciones_a_proveedoresRecord::set_pe_hostname(QString pe_hostname)
{
	setProperty("pe_hostname",pe_hostname);
}

void bgp_publicaciones_a_proveedoresRecord::set_pe_ip(QString pe_ip)
{
	setProperty("pe_ip",pe_ip);
}

void bgp_publicaciones_a_proveedoresRecord::set_peer_ip(QString peer_ip)
{
	setProperty("peer_ip",peer_ip);
}

void bgp_publicaciones_a_proveedoresRecord::set_peer_tipo(QString peer_tipo)
{
	setProperty("peer_tipo",peer_tipo);
}

void bgp_publicaciones_a_proveedoresRecord::set_prepends(QString prepends)
{
	setProperty("prepends",prepends);
}

void bgp_publicaciones_a_proveedoresRecord::set_proveedor(QString proveedor)
{
	setProperty("proveedor",proveedor);
}

void bgp_publicaciones_a_proveedoresRecord::set_red(QString red)
{
	setProperty("red",red);
}

QString bgp_publicaciones_a_proveedoresRecord::asn()
{
	return property("asn").toString();
}

QString bgp_publicaciones_a_proveedoresRecord::comunidad_base()
{
	return property("comunidad_base").toString();
}

int bgp_publicaciones_a_proveedoresRecord::grupo()
{
	return property("grupo").toInt();
}

QString bgp_publicaciones_a_proveedoresRecord::interface()
{
	return property("interface").toString();
}

QString bgp_publicaciones_a_proveedoresRecord::pe_hostname()
{
	return property("pe_hostname").toString();
}

QString bgp_publicaciones_a_proveedoresRecord::pe_ip()
{
	return property("pe_ip").toString();
}

QString bgp_publicaciones_a_proveedoresRecord::peer_ip()
{
	return property("peer_ip").toString();
}

QString bgp_publicaciones_a_proveedoresRecord::peer_tipo()
{
	return property("peer_tipo").toString();
}

QString bgp_publicaciones_a_proveedoresRecord::prepends()
{
	return property("prepends").toString();
}

QString bgp_publicaciones_a_proveedoresRecord::proveedor()
{
	return property("proveedor").toString();
}

QString bgp_publicaciones_a_proveedoresRecord::red()
{
	return property("red").toString();
}

bgp_publicaciones_a_proveedoresRecord &bgp_publicaciones_a_proveedoresRecord::operator=(const bgp_publicaciones_a_proveedoresRecord &other)
{
	clone(other);
	return *this;
}
