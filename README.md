# MongoDB-Access
MongoDB access with VS2017 C++

- MongoDB C++ Client Setup
	- mongo-cxx-driver setup
		- https://stackoverflow.com/questions/40040998/how-to-set-up-project-with-mongodb-cxx-driver-in-visual-studio
		- setup vcpkg
			- execute vcpkg/bootstrap-vcpkg.bat
			- vcpkg.exe integrate install
		- setup mongo-cxx-driver
			- vcpkg.exe install mongo-cxx-driver
			- vcpkg.exe integrate install
- delete collection all data
	- https://poiemaweb.com/mongdb-basics-shell-crud
	- setup mongodb shell
		- download
			- https://docs.mongodb.com/manual/mongo/
				- https://www.mongodb.com/try/download/enterprise
					- Tools -> MongoDB Shell
	- mongodb shell (mongosh)
		- use navi  (select database)
		- db.journey_date.remove({})
		- or db.journey_date.deleteMany({})
			- simple display

