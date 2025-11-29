from setuptools import find_packages, setup

package_name = 'rover_navigation'

setup(
    name=package_name,
    version='0.0.0',
    packages=find_packages(exclude=['test']),
    data_files=[
        ('share/ament_index/resource_index/packages',
            ['resource/' + package_name]),
        ('share/' + package_name, ['package.xml']),
        ('share/' + package_name + '/launch', [
            'launch/bringup.launch.py',
            'launch/camera.launch.py',
            'launch/navigation.launch.py',
            'launch/slam.launch.py',
            'launch/teleop.launch.py',
        ]),
    ],
    install_requires=['setuptools'],
    zip_safe=True,
    maintainer='thasinduwickrama',
    maintainer_email='thasinduwickrama12@gmail.com',
    description='TODO: Package description',
    license='TODO: License declaration',
    tests_require=['pytest'],
    entry_points={
        'console_scripts': [
        ],
    },
)
