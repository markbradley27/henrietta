-- Script requires that the henrietta db and henrietta user exist.
\c henrietta

CREATE TABLE IF NOT EXISTS enviro (
	timestamp timestamp,
	aqi_pm25_standard_5_m_avg integer,
	temp_c_5_m_avg float,
	humidity_5_m_avg float
);
GRANT SELECT, INSERT ON enviro TO henrietta;
