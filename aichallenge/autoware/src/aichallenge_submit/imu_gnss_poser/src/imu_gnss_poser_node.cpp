#include "rclcpp/rclcpp.hpp"
#include "geometry_msgs/msg/pose_with_covariance_stamped.hpp"
#include "sensor_msgs/msg/imu.hpp"

class ImuGnssPoser : public rclcpp::Node
{

public:
    ImuGnssPoser() : Node("imu_gnss_poser")
    {
        auto qos = rclcpp::QoS(rclcpp::KeepLast(10)).reliable();
        pub_pose_ = this->create_publisher<geometry_msgs::msg::PoseWithCovarianceStamped>("/localization/imu_gnss_poser/pose_with_covariance", qos);
        sub_gnss_ = this->create_subscription<geometry_msgs::msg::PoseWithCovarianceStamped>(
            "/sensing/gnss/pose_with_covariance", qos,
            std::bind(&ImuGnssPoser::gnss_callback, this, std::placeholders::_1));
        sub_imu_ = this->create_subscription<sensor_msgs::msg::Imu>(
            "/sensing/imu/imu_raw", qos,
            std::bind(&ImuGnssPoser::imu_callback, this, std::placeholders::_1));
    }

private:

    void gnss_callback(const geometry_msgs::msg::PoseWithCovarianceStamped::SharedPtr msg) {
        msg->pose.pose.orientation.x = imu_msg_.orientation.x;
        msg->pose.pose.orientation.y = imu_msg_.orientation.y;
        msg->pose.pose.orientation.z = imu_msg_.orientation.z;
        msg->pose.pose.orientation.w = imu_msg_.orientation.w;
        pub_pose_->publish(*msg);
    }

    void imu_callback(sensor_msgs::msg::Imu::SharedPtr msg) {
        imu_msg_ = *msg;
    }

    rclcpp::Publisher<geometry_msgs::msg::PoseWithCovarianceStamped>::SharedPtr pub_pose_;
    rclcpp::Subscription<geometry_msgs::msg::PoseWithCovarianceStamped>::SharedPtr sub_gnss_;
    rclcpp::Subscription<sensor_msgs::msg::Imu>::SharedPtr sub_imu_;
    sensor_msgs::msg::Imu imu_msg_;
};

int main(int argc, char const *argv[])
{
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<ImuGnssPoser>());
    rclcpp::shutdown();
    return 0;
}
