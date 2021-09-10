CREATE TABLE IF NOT EXISTS `pcp_region` (
  `id` INT NOT NULL AUTO_INCREMENT,
  `region` VARCHAR(45) NULL,
  PRIMARY KEY (`id`))
ENGINE = InnoDB;

INSERT INTO `pcp_region` (`id`, `region`) VALUES (NULL, 'NapCentroamerica'), (NULL, 'Colombia') 

ALTER TABLE `pcp_pais` ADD `pcp_region_id` INT NOT NULL AFTER `pais`; 

ALTER TABLE `unet`.`pcp_pais` ADD INDEX `fk_pcp_pais_pcp_region1_idx` (`pcp_region_id`); 

UPDATE `pcp_pais` SET `pcp_region_id` = '1' WHERE `pcp_pais`.`id` = 1; 
UPDATE `pcp_pais` SET `pcp_region_id` = '1' WHERE `pcp_pais`.`id` = 2; 
UPDATE `pcp_pais` SET `pcp_region_id` = '1' WHERE `pcp_pais`.`id` = 3; 
UPDATE `pcp_pais` SET `pcp_region_id` = '2' WHERE `pcp_pais`.`id` = 4; 
UPDATE `pcp_pais` SET `pcp_region_id` = '1' WHERE `pcp_pais`.`id` = 5; 


ALTER TABLE `pcp_pais` ADD CONSTRAINT `fk_pcp_pais_pcp_region1` FOREIGN KEY (`pcp_region_id`) REFERENCES `pcp_region`(`id`) ON DELETE NO ACTION ON UPDATE NO ACTION; 





ALTER TABLE `pcp_pe` ADD `pcp_pais_id` TINYINT(3) UNSIGNED NOT NULL AFTER `ip`; 

INSERT INTO `pcp_region` (`id`, `region`) VALUES (NULL, 'Sudamerica'), (NULL, 'Peru'); 

INSERT INTO `pcp_pais` (`id`, `pais`, `pcp_region_id`) VALUES (NULL, 'El Salvador', '1'), (NULL, 'Panama', '1') 
INSERT INTO `pcp_pais` (`id`, `pais`, `pcp_region_id`) VALUES (NULL, 'Nicaragua', '1'), (NULL, 'Argentina', '3'); 
INSERT INTO `pcp_pais` (`id`, `pais`, `pcp_region_id`) VALUES (NULL, 'Peru', '4'); 


UPDATE `pcp_pe` SET `pcp_pais_id` = '5' WHERE `pcp_pe`.`id` = 1; 
UPDATE `pcp_pe` SET `pcp_pais_id` = '6' WHERE `pcp_pe`.`id` = 2; 
UPDATE `pcp_pe` SET `pcp_pais_id` = '1' WHERE `pcp_pe`.`id` = 3; 
UPDATE `pcp_pe` SET `pcp_pais_id` = '7' WHERE `pcp_pe`.`id` = 4; 
UPDATE `pcp_pe` SET `pcp_pais_id` = '2' WHERE `pcp_pe`.`id` = 5; 
UPDATE `pcp_pe` SET `pcp_pais_id` = '5' WHERE `pcp_pe`.`id` = 6; 
UPDATE `pcp_pe` SET `pcp_pais_id` = '2' WHERE `pcp_pe`.`id` = 7; 
UPDATE `pcp_pe` SET `pcp_pais_id` = '3' WHERE `pcp_pe`.`id` = 8; 
UPDATE `pcp_pe` SET `pcp_pais_id` = '5' WHERE `pcp_pe`.`id` = 9; 
UPDATE `pcp_pe` SET `pcp_pais_id` = '3' WHERE `pcp_pe`.`id` = 10; 
UPDATE `pcp_pe` SET `pcp_pais_id` = '3' WHERE `pcp_pe`.`id` = 11; 
UPDATE `pcp_pe` SET `pcp_pais_id` = '1' WHERE `pcp_pe`.`id` = 12; 
UPDATE `pcp_pe` SET `pcp_pais_id` = '1' WHERE `pcp_pe`.`id` = 13; 
UPDATE `pcp_pe` SET `pcp_pais_id` = '8' WHERE `pcp_pe`.`id` = 14; 
UPDATE `pcp_pe` SET `pcp_pais_id` = '9' WHERE `pcp_pe`.`id` = 15; 
UPDATE `pcp_pe` SET `pcp_pais_id` = '10' WHERE `pcp_pe`.`id` = 16; 
UPDATE `pcp_pe` SET `pcp_pais_id` = '4' WHERE `pcp_pe`.`id` = 17; 
UPDATE `pcp_pe` SET `pcp_pais_id` = '4' WHERE `pcp_pe`.`id` = 18; 
UPDATE `pcp_pe` SET `pcp_pais_id` = '4' WHERE `pcp_pe`.`id` = 19; 
UPDATE `pcp_pe` SET `pcp_pais_id` = '4' WHERE `pcp_pe`.`id` = 20; 
UPDATE `pcp_pe` SET `ip` = '172.16.30.209' WHERE `pcp_pe`.`id` = 20; 
UPDATE `pcp_pe` SET `ip` = '172.16.30.67' WHERE `pcp_pe`.`id` = 17;


INSERT INTO `pcp_pe` (`id`, `nombre`, `ip`, `pcp_pais_id`) VALUES (NULL, 'COSABUCBUCRTCUFN02', '172.17.28.188', '4'); 
INSERT INTO `pcp_pe` (`id`, `nombre`, `ip`, `pcp_pais_id`) VALUES (NULL, 'COCDBOGJUPRTCUFN01', '172.17.28.209', '4'), (NULL, 'COCDBOGTERRTCUFN02', '172.17.28.219', '4'); 
INSERT INTO `pcp_pe` (`id`, `nombre`, `ip`, `pcp_pais_id`) VALUES (NULL, 'COBOCARBQ2RTCUFN01', '172.16.30.69', '4'), (NULL, 'COANMEDSDIRTCUFN01', '172.17.28.220', '4'); 
INSERT INTO `pcp_pe` (`id`, `nombre`, `ip`, `pcp_pais_id`) VALUES (NULL, 'COCEVALVLLRTCUFN01', '172.17.28.247', '4'); 

INSERT INTO `pcp_proveedor` (`id`, `proveedor`, `logo_path`) VALUES (NULL, 'IFX-Nacional', NULL); 
INSERT INTO `pcp_proveedor` (`id`, `proveedor`, `logo_path`) VALUES (NULL, 'Globenet', NULL); 
INSERT INTO `pcp_proveedor` (`id`, `proveedor`, `logo_path`) VALUES (NULL, 'Century', NULL), (NULL, 'Columbus', NULL); 

UPDATE `pcp_peer` SET `estado` = '0' WHERE `pcp_peer`.`id` = 18; 
UPDATE `pcp_peer` SET `estado` = '0' WHERE `pcp_peer`.`id` = 19; 
UPDATE `pcp_peer` SET `estado` = '0' WHERE `pcp_peer`.`id` = 20; 
UPDATE `pcp_peer` SET `estado` = '0' WHERE `pcp_peer`.`id` = 21; 


ALTER TABLE `unet`.`pcp_pe` ADD INDEX `fk_pcp_pe_pcp_pais1_idx` (`pcp_pais_id`); 
ALTER TABLE `pcp_pe` ADD CONSTRAINT `fk_pcp_pe_pcp_pais1` FOREIGN KEY (`pcp_pais_id`) REFERENCES `pcp_pais`(`id`) ON DELETE NO ACTION ON UPDATE NO ACTION; 



INSERT INTO `pcp_peer` (`id`, `pcp_pais_id`, `pcp_peer_tipo_id`, `pcp_proveedor_id`, `ip_peer`, `ip_pe`, `vrf`, `comunidad_base`, `estado`) VALUES (NULL, '4', '2', '4', '190.61.62.62', '172.17.28.12', 'INTERNET', '52468:2016', '1'); 
INSERT INTO `pcp_peer` (`id`, `pcp_pais_id`, `pcp_peer_tipo_id`, `pcp_proveedor_id`, `ip_peer`, `ip_pe`, `vrf`, `comunidad_base`, `estado`) VALUES (NULL, '4', '2', '1', '190.61.62.80', '172.17.28.12', 'INTERNET', '52468:2025', '1'); 
INSERT INTO `pcp_peer` (`id`, `pcp_pais_id`, `pcp_peer_tipo_id`, `pcp_proveedor_id`, `ip_peer`, `ip_pe`, `vrf`, `comunidad_base`, `estado`) VALUES (NULL, '4', '2', '5', '181.78.28.9', '172.17.28.12', 'INTERNET', '52468:2094', '1'); 
INSERT INTO `pcp_peer` (`id`, `pcp_pais_id`, `pcp_peer_tipo_id`, `pcp_proveedor_id`, `ip_peer`, `ip_pe`, `vrf`, `comunidad_base`, `estado`) VALUES (NULL, '4', '2', '5', '181.78.28.1', '172.17.28.209', 'INTERNET', '52468:2096', '1'); 
INSERT INTO `pcp_peer` (`id`, `pcp_pais_id`, `pcp_peer_tipo_id`, `pcp_proveedor_id`, `ip_peer`, `ip_pe`, `vrf`, `comunidad_base`, `estado`) VALUES (NULL, '4', '1', '11', '172.17.240.1', '172.16.30.67', 'INTERNET', '52468:4021', '1'); 
INSERT INTO `pcp_peer` (`id`, `pcp_pais_id`, `pcp_peer_tipo_id`, `pcp_proveedor_id`, `ip_peer`, `ip_pe`, `vrf`, `comunidad_base`, `estado`) VALUES (NULL, '4', '1', '11', '138.186.20.190', '172.16.30.67', 'INTERNET', '52468:4021', '1'); 
INSERT INTO `pcp_peer` (`id`, `pcp_pais_id`, `pcp_peer_tipo_id`, `pcp_proveedor_id`, `ip_peer`, `ip_pe`, `vrf`, `comunidad_base`, `estado`) VALUES (NULL, '4', '1', '11', '172.28.28.6', '172.16.30.67', 'INTERNET', '52468:4022', '1'); 
INSERT INTO `pcp_peer` (`id`, `pcp_pais_id`, `pcp_peer_tipo_id`, `pcp_proveedor_id`, `ip_peer`, `ip_pe`, `vrf`, `comunidad_base`, `estado`) VALUES (NULL, '4', '1', '12', '138.0.40.133', '172.17.28.12', 'INTERNET', '52468:2151', '1'); 
INSERT INTO `pcp_peer` (`id`, `pcp_pais_id`, `pcp_peer_tipo_id`, `pcp_proveedor_id`, `ip_peer`, `ip_pe`, `vrf`, `comunidad_base`, `estado`) VALUES (NULL, '4', '1', '12', '138.0.42.134', '172.17.28.12', 'INTERNET', '52468:2152', '1'); 
INSERT INTO `pcp_peer` (`id`, `pcp_pais_id`, `pcp_peer_tipo_id`, `pcp_proveedor_id`, `ip_peer`, `ip_pe`, `vrf`, `comunidad_base`, `estado`) VALUES (NULL, '4', '1', '13', '204.199.115.177', '172.17.28.242', 'INTERNET', '52468:2185', '1'); 
INSERT INTO `pcp_peer` (`id`, `pcp_pais_id`, `pcp_peer_tipo_id`, `pcp_proveedor_id`, `ip_peer`, `ip_pe`, `vrf`, `comunidad_base`, `estado`) VALUES (NULL, '4', '1', '14', '190.242.173.38', '172.16.30.69', 'INTERNET', '52468:2197', '1'); 
INSERT INTO `pcp_peer` (`id`, `pcp_pais_id`, `pcp_peer_tipo_id`, `pcp_proveedor_id`, `ip_peer`, `ip_pe`, `vrf`, `comunidad_base`, `estado`) VALUES (NULL, '4', '1', '1', '157.240.74.72', '172.17.28.219', 'INTERNET', '52468:4023', '1'); 
INSERT INTO `pcp_peer` (`id`, `pcp_pais_id`, `pcp_peer_tipo_id`, `pcp_proveedor_id`, `ip_peer`, `ip_pe`, `vrf`, `comunidad_base`, `estado`) VALUES (NULL, '4', '1', '1', '157.240.80.90', '172.17.28.219', 'INTERNET', '52468:4028', '1'); 
INSERT INTO `pcp_peer` (`id`, `pcp_pais_id`, `pcp_peer_tipo_id`, `pcp_proveedor_id`, `ip_peer`, `ip_pe`, `vrf`, `comunidad_base`, `estado`) VALUES (NULL, '4', '1', '9', '104.44.196.49', '172.17.28.219', 'INTERNET', '52468:4063', '1'); 
INSERT INTO `pcp_peer` (`id`, `pcp_pais_id`, `pcp_peer_tipo_id`, `pcp_proveedor_id`, `ip_peer`, `ip_pe`, `vrf`, `comunidad_base`, `estado`) VALUES (NULL, '4', '1', '9', '104.44.196.167', '172.17.28.219', 'INTERNET', '52468:4064', '1'); 
INSERT INTO `pcp_peer` (`id`, `pcp_pais_id`, `pcp_peer_tipo_id`, `pcp_proveedor_id`, `ip_peer`, `ip_pe`, `vrf`, `comunidad_base`, `estado`) VALUES (NULL, '4', '1', '4', '181.78.28.30', '172.17.28.219', 'INTERNET', '52468:2010', '1'); 
INSERT INTO `pcp_peer` (`id`, `pcp_pais_id`, `pcp_peer_tipo_id`, `pcp_proveedor_id`, `ip_peer`, `ip_pe`, `vrf`, `comunidad_base`, `estado`) VALUES (NULL, '4', '1', '4', '74.125.147.152', '172.17.28.219', 'INTERNET', '52468:2290', '1'); 
INSERT INTO `pcp_peer` (`id`, `pcp_pais_id`, `pcp_peer_tipo_id`, `pcp_proveedor_id`, `ip_peer`, `ip_pe`, `vrf`, `comunidad_base`, `estado`) VALUES (NULL, '4', '1', '4', '142.250.166.246', '172.17.28.219', 'INTERNET', '52468:2292', '1'); 

