import numpy as np
import matplotlib.pyplot as plt
from scipy.optimize import curve_fit

# 1. 读取 C++ 导出的数据 (两列：第一列时间，第二列角度)
data = np.loadtxt("result2/task2_fit/angle_data.txt")
t = data[:, 0]          # 第1列是时间
angle_deg = data[:, 1]  # 第2列是角度

# 2. 角度解包裹
angle_rad = np.deg2rad(angle_deg)
angle_rad = np.unwrap(angle_rad)

# 3. 正弦拟合模型
def model(t, A, b, Omega, phi):
    return b + A * np.sin(Omega * t + phi)

# 4. 进行拟合 (初始猜测：幅度1，偏置0，角速度约 2pi/24，相位0)
initial_guess = [1.0, 0.0, 2 * np.pi / 24, 0.0]
popt, pcov = curve_fit(model, t, angle_rad, p0=initial_guess)
A_fit, b_fit, Omega_fit, phi_fit = popt

print(f"拟合参数: A={A_fit:.4f}, b={b_fit:.4f}, Omega={Omega_fit:.4f}, phi={phi_fit:.4f}")

# 5. 计算残差和 RMSE
angle_fit = model(t, *popt)
residuals = angle_rad - angle_fit
rmse = np.sqrt(np.mean(residuals**2))
print(f"RMSE: {rmse:.4f} rad")

# 6. 绘制 fit_comparison.png
plt.figure()
plt.plot(t, angle_rad, 'b.', label='Actual Data', markersize=2)
plt.plot(t, angle_fit, 'r-', label='Fitted Curve')
plt.xlabel('Time (s)')
plt.ylabel('Angle (rad)')
plt.legend()
plt.savefig("result2/task2_fit/fit_comparison.png")
plt.close()

# 7. 绘制 residuals.png
plt.figure()
plt.plot(t, residuals, 'g.', markersize=2)
plt.xlabel('Time (s)')
plt.ylabel('Residuals (rad)')
plt.savefig("result2/task2_fit/residuals.png")
plt.close()

# 8. 绘制 angular_velocity.png
omega = np.gradient(angle_rad, t)
plt.figure()
plt.plot(t, omega, 'm.', markersize=2)
plt.xlabel('Time (s)')
plt.ylabel('Angular Velocity (rad/s)')
plt.savefig("result2/task2_fit/angular_velocity.png")
plt.close()

print("所有图表已保存到 result2/task2_fit/ 目录下！")