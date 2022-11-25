CREATE TABLE `user` (
  `id` bigint NOT NULL AUTO_INCREMENT,
  `last_login` timestamp(2) NULL DEFAULT NULL,
  `creation_date` datetime(2) DEFAULT NULL,
  `userID` bigint NOT NULL,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB AUTO_INCREMENT=11 DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci