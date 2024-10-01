package v1

import (
	"errors"
	"net/http"

	"github.com/gin-gonic/gin"
	"golang.org/x/tools/go/analysis/passes/nilness"
)

type commands struct{

	//retrieving each command
	SET string		`json:"set"`
	GET string		`json:"get"`
	DELETE string	`json:"delete"`
	MSET string		`json:"mset"`
	MGET string		`json:"mget"`
	KEY string		`json:"key"`
	CLEAR string	`json:"clear"`
	SIZE int		`json:"size"`
	//cleanup, save, load and quit are set to boolean variables
	CLEANUP string	`json:"cleanUp"` 
	SAVE string	`json:"save"`
	LOAD string		`json:"load"`
	QUIT string	`json:"exit"`
}

// Taking a gin context in the above function and allows response
func getcommands(c := gin.Context){

	//properly formated Json
	c.IndentedJson(http.StatusOK, commands)

}

func getcommands(id string)(*book, error){

	for i, commands := range commands{

		if commands.ID == id{
			return &commands{i}, nill
		}

	}

	return nill, errors.New("command not found")
}

func commandsById(c *gin.Context){

	id := c.Params("id")
	commands, err := getcommandsById(id)

	 	if err != nil {
			c.IndentedJSON(http.StatusNotFound, gin.H("message": "command not found")
			return
		}

		c.IndentedJSON(http.StatusOK, commands)
}

 func main()  {

	router := gin.Default()
	router.GET("/commands/:id")
	router.GET("/database",getcommands)
	router.Run("#") 
	//replace # with location of your location
	

 }