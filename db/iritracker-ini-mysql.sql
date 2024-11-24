-- phpMyAdmin SQL Dump
-- version 5.2.1
-- https://www.phpmyadmin.net/
--
-- Máy chủ: 127.0.0.1
-- Thời gian đã tạo: Th10 24, 2024 lúc 06:23 AM
-- Phiên bản máy phục vụ: 10.4.32-MariaDB
-- Phiên bản PHP: 8.2.12

SET SQL_MODE = "NO_AUTO_VALUE_ON_ZERO";
START TRANSACTION;
SET time_zone = "+00:00";


/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!40101 SET NAMES utf8mb4 */;

--
-- Cơ sở dữ liệu: `iri-tracker-standard`
--

-- --------------------------------------------------------

--
-- Cấu trúc bảng cho bảng `attendance_event`
--

CREATE TABLE `attendance_event` (
  `attendance_event_id` int(11) NOT NULL,
  `type` varchar(50) NOT NULL,
  `date` int(11) NOT NULL,
  `user_id` varchar(36) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci;

-- --------------------------------------------------------

--
-- Cấu trúc bảng cho bảng `bulletin_board`
--

CREATE TABLE `bulletin_board` (
  `bulletin_board_id` int(11) NOT NULL,
  `title` text NOT NULL,
  `content` text NOT NULL,
  `start_date` int(11) DEFAULT NULL,
  `end_date` int(11) DEFAULT NULL,
  `is_active` int(11) DEFAULT NULL,
  `is_high_priority` int(11) DEFAULT NULL,
  `user_id` varchar(36) NOT NULL,
  `created_at` timestamp NOT NULL DEFAULT current_timestamp(),
  `updated_at` timestamp NOT NULL DEFAULT current_timestamp() ON UPDATE current_timestamp()
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci;

-- --------------------------------------------------------

--
-- Cấu trúc bảng cho bảng `department`
--

CREATE TABLE `department` (
  `department_id` int(11) NOT NULL,
  `name` varchar(50) NOT NULL,
  `description` text DEFAULT NULL,
  `created_at` timestamp NOT NULL DEFAULT current_timestamp(),
  `updated_at` timestamp NOT NULL DEFAULT current_timestamp(),
  `is_active` int(11) DEFAULT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci;

--
-- Đang đổ dữ liệu cho bảng `department`
--

INSERT INTO `department` (`department_id`, `name`, `description`, `created_at`, `updated_at`, `is_active`) VALUES
(1, 'Others', 'Other', NOW(), NOW(), 1);

-- --------------------------------------------------------

--
-- Cấu trúc bảng cho bảng `exception`
--

CREATE TABLE `exception` (
  `exception_id` int(11) NOT NULL,
  `name` varchar(50) NOT NULL,
  `paid_hours` int(11) NOT NULL,
  `paid_coefficient` double NOT NULL,
  `work_coefficient` double NOT NULL,
  `is_overtime` int(11) NOT NULL,
  `created_at` timestamp NOT NULL DEFAULT current_timestamp(),
  `updated_at` timestamp NOT NULL DEFAULT current_timestamp() ON UPDATE current_timestamp()
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci;

--
-- Đang đổ dữ liệu cho bảng `exception`
--

INSERT INTO `exception` (`exception_id`, `name`, `paid_hours`, `paid_coefficient`, `work_coefficient`, `is_overtime`, `created_at`, `updated_at`) VALUES
(1, 'Sickly', 28800, 1, 1, 1, NOW(), NOW()),
(2, 'Vaction', 28800, 1, 1, 1, NOW(), NOW()),
(3, 'Holiday', 28800, 1, 1, 1, NOW(), NOW());

-- --------------------------------------------------------

--
-- Cấu trúc bảng cho bảng `role`
--

CREATE TABLE `role` (
  `role_id` int(11) NOT NULL,
  `name` varchar(50) NOT NULL,
  `description` text DEFAULT NULL,
  `created_at` timestamp NOT NULL DEFAULT current_timestamp(),
  `updated_at` timestamp NOT NULL DEFAULT current_timestamp() ON UPDATE current_timestamp()
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci;

--
-- Đang đổ dữ liệu cho bảng `role`
--

INSERT INTO `role` (`role_id`, `name`, `description`, `created_at`, `updated_at`) VALUES
(1, 'Admin', 'admin', NOW(), NOW()),
(2, 'User', 'user', NOW(), NOW());

-- --------------------------------------------------------

--
-- Cấu trúc bảng cho bảng `user`
--

CREATE TABLE `user` (
  `user_id` varchar(36) NOT NULL,
  `first_name` varchar(50) DEFAULT NULL,
  `last_name` varchar(50) DEFAULT NULL,
  `is_password` int(11) DEFAULT NULL,
  `password` text DEFAULT NULL,
  `department_id` int(11) DEFAULT NULL,
  `date_of_birth` varchar(50) DEFAULT NULL,
  `start_working_date` int(11) DEFAULT NULL,
  `is_active` int(11) NOT NULL,
  `avatar` blob DEFAULT NULL,
  `email` varchar(100) DEFAULT NULL,
  `phone` varchar(15) DEFAULT NULL,
  `cell_phone` varchar(15) DEFAULT NULL,
  `address` text DEFAULT NULL,
  `eye_right` blob DEFAULT NULL,
  `eye_left` blob DEFAULT NULL,
  `role_id` int(11) NOT NULL,
  `created_at` timestamp NOT NULL DEFAULT current_timestamp(),
  `updated_at` timestamp NOT NULL DEFAULT current_timestamp() ON UPDATE current_timestamp()
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci;

-- --------------------------------------------------------

--
-- Cấu trúc bảng cho bảng `user_exception`
--

CREATE TABLE `user_exception` (
  `user_exception_id` int(11) NOT NULL,
  `user_id` varchar(36) NOT NULL,
  `exception_id` int(11) NOT NULL,
  `start_date` int(11) NOT NULL,
  `end_date` int(11) NOT NULL,
  `is_all_date` int(11) NOT NULL,
  `day_of_week` int(11) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci;

--
-- Chỉ mục cho các bảng đã đổ
--

--
-- Chỉ mục cho bảng `attendance_event`
--
ALTER TABLE `attendance_event`
  ADD PRIMARY KEY (`attendance_event_id`),
  ADD KEY `user_id` (`user_id`);

--
-- Chỉ mục cho bảng `bulletin_board`
--
ALTER TABLE `bulletin_board`
  ADD PRIMARY KEY (`bulletin_board_id`),
  ADD KEY `user_id` (`user_id`);

--
-- Chỉ mục cho bảng `department`
--
ALTER TABLE `department`
  ADD PRIMARY KEY (`department_id`),
  ADD UNIQUE KEY `name` (`name`);

--
-- Chỉ mục cho bảng `exception`
--
ALTER TABLE `exception`
  ADD PRIMARY KEY (`exception_id`),
  ADD UNIQUE KEY `name` (`name`);

--
-- Chỉ mục cho bảng `role`
--
ALTER TABLE `role`
  ADD PRIMARY KEY (`role_id`),
  ADD UNIQUE KEY `name` (`name`);

--
-- Chỉ mục cho bảng `user`
--
ALTER TABLE `user`
  ADD PRIMARY KEY (`user_id`),
  ADD KEY `department_id` (`department_id`),
  ADD KEY `role_id` (`role_id`);

--
-- Chỉ mục cho bảng `user_exception`
--
ALTER TABLE `user_exception`
  ADD PRIMARY KEY (`user_exception_id`),
  ADD KEY `user_id` (`user_id`),
  ADD KEY `exception_id` (`exception_id`);

--
-- AUTO_INCREMENT cho các bảng đã đổ
--

--
-- AUTO_INCREMENT cho bảng `attendance_event`
--
ALTER TABLE `attendance_event`
  MODIFY `attendance_event_id` int(11) NOT NULL AUTO_INCREMENT;

--
-- AUTO_INCREMENT cho bảng `bulletin_board`
--
ALTER TABLE `bulletin_board`
  MODIFY `bulletin_board_id` int(11) NOT NULL AUTO_INCREMENT;

--
-- AUTO_INCREMENT cho bảng `department`
--
ALTER TABLE `department`
  MODIFY `department_id` int(11) NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=2;

--
-- AUTO_INCREMENT cho bảng `exception`
--
ALTER TABLE `exception`
  MODIFY `exception_id` int(11) NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=4;

--
-- AUTO_INCREMENT cho bảng `role`
--
ALTER TABLE `role`
  MODIFY `role_id` int(11) NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=3;

--
-- AUTO_INCREMENT cho bảng `user_exception`
--
ALTER TABLE `user_exception`
  MODIFY `user_exception_id` int(11) NOT NULL AUTO_INCREMENT;

--
-- Các ràng buộc cho các bảng đã đổ
--

--
-- Các ràng buộc cho bảng `attendance_event`
--
ALTER TABLE `attendance_event`
  ADD CONSTRAINT `attendance_event_ibfk_1` FOREIGN KEY (`user_id`) REFERENCES `user` (`user_id`);

--
-- Các ràng buộc cho bảng `bulletin_board`
--
ALTER TABLE `bulletin_board`
  ADD CONSTRAINT `bulletin_board_ibfk_1` FOREIGN KEY (`user_id`) REFERENCES `user` (`user_id`);

--
-- Các ràng buộc cho bảng `user`
--
ALTER TABLE `user`
  ADD CONSTRAINT `user_ibfk_1` FOREIGN KEY (`department_id`) REFERENCES `department` (`department_id`),
  ADD CONSTRAINT `user_ibfk_2` FOREIGN KEY (`role_id`) REFERENCES `role` (`role_id`);

--
-- Các ràng buộc cho bảng `user_exception`
--
ALTER TABLE `user_exception`
  ADD CONSTRAINT `user_exception_ibfk_1` FOREIGN KEY (`user_id`) REFERENCES `user` (`user_id`),
  ADD CONSTRAINT `user_exception_ibfk_2` FOREIGN KEY (`exception_id`) REFERENCES `exception` (`exception_id`);
COMMIT;

/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
